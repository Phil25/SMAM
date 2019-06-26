#include "common.h"

#include "../utils/smfs.h"

auto Command::remove(const Opts& opts) noexcept -> ExitCode
{
    namespace fs = std::filesystem;

    auto addons = opts.getAddons();
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

    for (const auto& addon : addons)
    {
        if (!SMFS::isInstalled(addon))
        {
            out(Ch::Warn) << "Addon not installed: " << addon << cr;
            continue;
        }

        out(Ch::Info) << Col::yellow << "Removing " << addon << "..."
                      << Col::reset << cr;

        for (const auto& file : SMFS::getFiles(addon))
        {
            switch (SMFS::removeFile(file))
            {
                case SMFS::DeleteResult::NotExists:
                    out()
                        << "Skipping non-existent file: " << file << cr;
                    break;

                case SMFS::DeleteResult::Shared:
                    out() << "Skipping shared file: " << file << cr;
                    break;

                case SMFS::DeleteResult::OK: out() << file << cr; break;
            }
        }

        SMFS::eraseAddon(addon);
    }

    if (!SMFS::writeData())
    {
        out(Ch::Error) << "Cannot write local addon metadata." << cr;
        return ExitCode::WriteError;
    }

    return ExitCode::OK;
}
