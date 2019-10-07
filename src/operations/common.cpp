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

CheckSMRoot::CheckSMRoot(const LoggerPtr& logger,
                         CommonContext&   context) noexcept
    : Operation(logger, context)
{
}

void CheckSMRoot::Run() noexcept
{
    auto dest = GetContext().options->Destination().value_or("");
    auto root = path::FindSMRoot(dest);

    if (root)
    {
        GetContext().root = root.value();
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
    if (path.empty()) path = GetContext().root / cache;

    if (!path::HasReadAndWritePermissions(path))
    {
        Fail("No read or write permissions.", ExitCode::NoPermissions);
    }

    if (!Addon::Load(path))
    {
        Fail("Failed to load installed addons.", ExitCode::BadCache);
    }
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
    }

    if (!Addon::Save(path))
    {
        Fail("Failed to save installed addons.", ExitCode::WriteError);
    }
}
}  // namespace smam
