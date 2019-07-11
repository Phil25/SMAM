#include "common.h"

#include <smfs.h>

auto Command::info(const Opts& opts) noexcept -> ExitCode
{
    namespace fs = std::filesystem;

    auto dest = opts.getDestination().value_or("");
    auto root = SMFS::Path::findRoot(dest);

    if (root)
    {
        fs::current_path(root.value());
    }
    else
    {
        out(Ch::Error) << "SourceMod root not found." << cr;
        return ExitCode::NoSMRoot;
    }

    if (!SMFS::Data::load())
    {
        out(Ch::Error) << "No read/write premissions." << cr;
        return ExitCode::NoPermissions;
    }

    const auto& filter = opts.getAddons();

    if (filter.empty())
    {
        out(Ch::Info) << "Installed addons:" << cr;

        SMFS::Addon::getInstalled([](const std::string& addon) {
            out() << addon << " (" << SMFS::Addon::files(addon).size()
                  << " file(s))" << cr;
        });

        return ExitCode::OK;
    }

    for (const auto& addon : filter)
    {
        if (!SMFS::Addon::isInstalled(addon))
        {
            out(Ch::Warn) << "Not installed: " << addon << cr;
            continue;
        }

        const auto& files = SMFS::Addon::files(addon);

        out(Ch::Info) << Col::green << addon << Col::reset << " ("
                      << files.size() << ')' << cr;

        for (const auto& file : files) out() << file << cr;
    }

    return ExitCode::OK;
}
