#include "common.h"

#include "../utils/smfs.h"
#include "helpers/report.h"

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
    namespace fs = std::filesystem;

    auto addons = opts.getAddons();
    if (addons.empty())
    {
        out(Ch::Error) << "No addons specified." << cr;
        return ExitCode::NoAddons;
    }

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

    auto report = removeAddons(addons);

    if (!SMFS::Data::save())
    {
        out(Ch::Error) << "Cannot write local addon metadata." << cr;
        return ExitCode::WriteError;
    }

    report.print();

    return ExitCode::OK;
}
