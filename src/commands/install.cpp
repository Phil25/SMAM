#include "common.h"

#include "../downloader.h"
#include "../installer.h"
#include "../utils/archive.h"
#include "../utils/smfs.h"

auto Command::install(const Opts& opts) noexcept -> ExitCode
{
    namespace fs = std::filesystem;

    const auto& addons = opts.getAddons();
    if (addons.empty())
    {
        out(Ch::Error) << "No addons specified." << cr;
        return ExitCode::NoAddons;
    }

    auto root = SMFS::findRoot(opts.getDestination().value_or(""));
    if (root)
    {
        fs::current_path(root.value());
    }
    else
    {
        out(Ch::Error) << "SourceMod root not found." << cr;
        return ExitCode::NoSMRoot;
    }

    if (!SMFS::loadData())
    {
        out(Ch::Error) << "No read/write premissions." << cr;
        return ExitCode::NoPermissions;
    }

    Downloader down;
    Database   db(down, opts.getDbUrl());
    Installer::initScrapers(down);
    db.precache(addons);
    int failed = 0;

    for (const auto& addon : addons)
    {
        if (SMFS::isInstalled(addon) && !opts.force())
        {
            out(Ch::Info) << "Already installed: " << addon << cr;
            continue;
        }

        if (!db.isPrecached(addon))
        {
            out(Ch::Error) << Col::red << "Not found: " << addon
                           << Col::reset << cr;

            ++failed;
            continue;
        }

        out(Ch::Info) << Col::green << "Installing " << addon << "..."
                      << Col::reset << cr;

        auto registerFile = [&](const fs::path& file) {
            if (!SMFS::prepare(file.parent_path()))
            {
                out(Ch::Warn) << "Ignoring " << file << cr;
                return false;
            }

            bool exists = SMFS::fs::exists(file);
            out(exists ? Ch::Warn : Ch::Std)
                << (exists ? "Overwriting " : "") << file << cr;

            SMFS::addFile(file, addon);
            return true;
        };

        bool success = Installer::setup(addon, db, [&](const File& f) {
            if (f.at == std::string::npos)
            {
                out(Ch::Error) << "Invalid addon format, please report "
                                  "to submitter."
                               << cr;
                return false;
            }

            fs::path file = f.path;
            file /= f.name;
            if (!registerFile(file)) return false;

            if (auto err = down.file(f.url, file); !err.empty())
            {
                out(Ch::Error) << err << cr;
                return false;
            }

            if (!Archive::valid(file))
                return true;  // no further action

            out(Ch::Info) << "Extracting " << f.name << "..." << cr;

            bool result = Archive::extract(
                file,
                [&](const fs::path& f) { return registerFile(f); });

            return result && SMFS::eraseFile(file, addon);
        });

        if (!success)
        {
            out(Ch::Error) << Col::red << "Failed to install " << addon
                           << Col::reset << cr;

            for (const auto& file : SMFS::getFiles(addon))
            {
                SMFS::removeFile(file);
            }

            SMFS::eraseAddon(addon);
            ++failed;
        }
    }

    if (!SMFS::writeData())
    {
        out(Ch::Error) << "Cannot write local addon metadata." << cr;
        return ExitCode::WriteError;
    }

    if (failed)
    {
        out(Ch::Error) << Col::red << failed
                       << " addon(s) failed to install." << Col::reset
                       << cr;
    }

    return ExitCode::OK;
}
