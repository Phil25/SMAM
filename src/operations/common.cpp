#include "common.h"

#include <data/addon.h>
#include <utils/path.h>

namespace smam
{
constexpr std::string_view cache = ".smamdata.json";

CommonContext::CommonContext(const OptionsPtr& options) noexcept
    : options(options)
{
}

CheckAddons::CheckAddons(const LoggerPtr& logger,
                         CommonContext&   context) noexcept
    : Operation(logger, context)
{
}

void CheckAddons::Run() noexcept
{
    if (GetContext().options->Addons().empty())
    {
        Fail("No addons specified.", ExitCode::NoAddons);
    }
}

GoToSMRoot::GoToSMRoot(const LoggerPtr& logger,
                       CommonContext&   context) noexcept
    : Operation(logger, context)
{
}

void GoToSMRoot::Run() noexcept
{
    auto dest = GetContext().options->Destination().value_or("");
    auto root = path::FindSMRoot(dest);

    if (root)
    {
        GetContext().root = root.value();
        std::filesystem::current_path(GetContext().root);
    }
    else
    {
        Fail("SourceMod root not found.", ExitCode::NoSMRoot);
    }
}

LoadAddons::LoadAddons(const LoggerPtr& logger, CommonContext& context,
                       std::filesystem::path path) noexcept
    : Operation(logger, context), path(std::move(path))
{
}

void LoadAddons::Run() noexcept
{
    assert(path.parent_path().empty() &&
           "LoadAddons path parameter must be in current directory");

    if (!path::HasReadAndWritePermissions(path))
    {
        Fail("No read or write permissions.", ExitCode::NoPermissions);
        return;
    }

    if (Addon::Load(path)) return;

    GetLogger()->Warning()
        << "Loading addons failed! Attempting to load backup." << cr;

    auto backup = path;
    backup.replace_extension("json.bak");

    if (!path::fs::exists(backup))
    {
        Fail("Backup does not exist.", ExitCode::BadCache);
        return;
    }

    if (!path::HasReadAndWritePermissions(backup))
    {
        Fail("No read or write permissions.", ExitCode::NoPermissions);
        return;
    }

    if (Addon::Load(backup)) return;

    Fail("Failed to load installed addons.", ExitCode::BadCache);
}

SaveAddons::SaveAddons(const LoggerPtr& logger, CommonContext& context,
                       std::filesystem::path path) noexcept
    : Operation(logger, context), path(std::move(path))
{
}

void SaveAddons::Run() noexcept
{
    if (!path::HasReadAndWritePermissions(path))
    {
        Fail("No read or write permissions.", ExitCode::NoPermissions);
        return;
    }

    if (!Addon::Save(path))
    {
        Fail("Failed to save installed addons.", ExitCode::WriteError);
        return;
    }

    auto backup = path;
    backup.replace_extension("json.bak");

    if (!path::HasReadAndWritePermissions(backup))
    {
        Fail("No read or write permissions.", ExitCode::NoPermissions);
        return;
    }

    if (Addon::Save(backup) && path::MD5(path) == path::MD5(backup))
    {
        return;
    }

    Fail("Failed to backup installed addons.", ExitCode::WriteError);
    path::fs::remove(backup);
}
}  // namespace smam
