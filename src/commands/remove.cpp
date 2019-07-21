#include "common.h"

#include <commands/helpers/report.h>
#include <smfs/addon.h>

auto removeAddon(Addon& addon) noexcept -> Report::Type
{
    out(Ch::Info) << Col::yellow << "Removing " << addon.id << "..."
                  << Col::reset << cr;

    // TODO: make this shorter
    addon.remove([](const auto& result) {
        if (result.second.empty())  // no error
        {
            out() << "Skipping " << result.second
                  << " file:" << result.first.get().raw() << cr;
        }
        else
        {
            out() << result.first.get().raw() << cr;
        }
    });

    return Report::Type::Removed;
}

auto removeAddons(const std::vector<std::string>& addons) noexcept
{
    Report report;

    for (const auto& id : addons)
    {
        if (auto addon = Addon::get(id))  // addon installed
        {
            removeAddon(*addon.value());
            report.insert(Report::Type::Removed, id);
        }
        else
        {
            out(Ch::Warn) << "Addon not installed: " << id << cr;
            report.insert(Report::Type::Skipped, id);
        }

        out << cr;
    }

    return report;
}

auto Command::remove(const Opts& opts) noexcept -> ExitCode
{
    auto addons = opts.getAddons();

    if (Common::noAddons(addons)) return ExitCode::NoAddons;
    if (Common::noSMRoot(opts)) return ExitCode::NoSMRoot;
    if (auto ret = Common::load(); ret) return ret;

    auto report = removeAddons(addons);

    if (!Common::save()) return ExitCode::WriteError;

    report.print();

    return ExitCode::OK;
}
