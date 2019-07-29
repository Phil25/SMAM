#include "common.h"

#include <smfs/addon.h>

auto Command::info(const Opts& opts) noexcept -> ExitCode
{
    if (Common::noSMRoot(opts)) return ExitCode::NoSMRoot;
    if (auto ret = Common::load(); ret) return ret;

    const auto& filter = opts.getAddons();

    if (filter.empty())
    {
        out(Ch::Info) << "Installed addons:" << cr;

        Addon::forEach([](const auto& addon) {
            out() << addon << " (" << addon->getFiles().size()
                  << " file(s))" << cr;
        });

        return ExitCode::OK;
    }

    for (const auto& id : filter)
    {
        auto addonOpt = Addon::get(id);

        if (!addonOpt)
        {
            out(Ch::Warn) << "Not installed: " << id << cr;
            continue;
        }

        auto addon = addonOpt.value();

        out(Ch::Info) << Col::green << id << Col::reset << " ("
                      << addon->getFiles().size() << ')' << cr;

        addon->forEachFile(
            [](const auto& file) { out() << file->raw() << cr; });
    }

    return ExitCode::OK;
}
