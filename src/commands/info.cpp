#include "common.h"

#include "../utils/smfs.h"

auto Command::info(const Opts& opts) noexcept -> ExitCode
{
    namespace fs = std::filesystem;

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

    const auto& filter = opts.getAddons();

    if (filter.empty())
    {
        out(Ch::Info) << "Installed addons:" << cr;

        SMFS::getInstalled([](const std::string& addon) {
            out() << addon << " (" << SMFS::getFiles(addon).size()
                  << " file(s))" << cr;
        });

        return ExitCode::OK;
    }

    for (const auto& addon : filter)
    {
        if (!SMFS::isInstalled(addon))
        {
            out(Ch::Warn) << "Not installed: " << addon << cr;
            continue;
        }

        const auto& files = SMFS::getFiles(addon);

        out(Ch::Info) << Col::green << addon << Col::reset << " ("
                      << files.size() << ')' << cr;

        for (const auto& file : files) out() << file << cr;
    }

    return ExitCode::OK;
}
