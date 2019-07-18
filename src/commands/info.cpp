#include "common.h"

#include <smfs.h>

auto Command::info(const Opts& opts) noexcept -> ExitCode
{
    if (Common::noSMRoot(opts)) return ExitCode::NoSMRoot;
    if (auto ret = Common::load(); ret) return ret;

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
