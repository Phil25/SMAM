#include "common.h"

#include <commands/helpers/report.h>
#include <smfs.h>

auto removeAddon(const std::string& addon) noexcept -> Report::Type
{
    if (!SMFS::Addon::isInstalled(addon))
    {
        out(Ch::Warn) << "Addon not installed: " << addon << cr << cr;
        return Report::Type::Skipped;
    }

    out(Ch::Info) << Col::yellow << "Removing " << addon << "..."
                  << Col::reset << cr;

    for (const auto& file : SMFS::Addon::files(addon))
    {
        switch (SMFS::File::remove(file))
        {
            case SMFS::DeleteResult::NotExists:
                out() << "Skipping non-existent file: " << file << cr;
                break;

            case SMFS::DeleteResult::Shared:
                out() << "Skipping shared file: " << file << cr;
                break;

            case SMFS::DeleteResult::OK: out() << file << cr; break;
        }
    }

    SMFS::Addon::erase(addon);
    out << cr;
    return Report::Type::Removed;
}

auto removeAddons(const std::vector<std::string>& addons) noexcept
{
    Report report;

    for (const auto& addon : addons)
    {
        report.insert(removeAddon(addon), addon);
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
