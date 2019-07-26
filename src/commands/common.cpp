#include "common.h"

#include <smfs/addon.h>
#include <smfs/path.h>

#include <filesystem>

const std::map<std::string_view, Command::fptr> Command::map{
    {"install", Command::install},  {"remove", Command::remove},
    {"uninstall", Command::remove}, {"info", Command::info},
    {"list", Command::info},        {"search", Command::search},
};

bool Command::exists(const std::string& command) noexcept
{
    return map.count(command);
}

auto Command::run(const std::string& command, const Opts& opts) noexcept
    -> ExitCode
{
    return map.at(command)(opts);
}

bool Common::noAddons(const std::vector<std::string>& addons) noexcept
{
    if (!addons.empty()) return false;

    out(Ch::Error) << "No addons specified." << cr;
    return true;
}

bool Common::noSMRoot(const Opts& opts) noexcept
{
    auto dest = opts.getDestination().value_or("");
    auto root = Path::findRoot(dest);

    if (root)
    {
        std::filesystem::current_path(root.value());
        return false;
    }
    else
    {
        out(Ch::Error) << "SourceMod root not found." << cr;
        return true;
    };
}

auto Common::load() noexcept -> ExitCode
{
    switch (Addon::load())
    {
        case Addon::LoadResult::NoAccess:
            out(Ch::Error) << "No read/write premissions." << cr;
            return ExitCode::NoPermissions;

        case Addon::LoadResult::Corrupted:
            out(Ch::Error)
                << "Cache corrupted. Attempting recovery..." << cr;
            return ExitCode::CorruptedCache;

        default:;  // SMFS::LoadResult::OK
    }

    return ExitCode::OK;
}

bool Common::save() noexcept
{
    if (Addon::save()) return true;

    out(Ch::Error) << "Cannot write local addon metadata." << cr;
    return false;
}
