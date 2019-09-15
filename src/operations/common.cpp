#include "common.h"

#include <data/addon.h>
#include <utils/path.h>

namespace smam
{
CheckAddons::CheckAddons(Logger& logger, CommonContext& context,
                         const Options& options) noexcept
    : Operation(logger, context), options(options)
{
}

void CheckAddons::Run() noexcept
{
    if (options.Addons().empty())
    {
        Fail("No addons specified.");
    }
}

CheckSMRoot::CheckSMRoot(Logger& logger, CommonContext& context,
                         const Options& options) noexcept
    : Operation(logger, context), options(options)
{
}

void CheckSMRoot::Run() noexcept
{
    auto dest = options.Destination().value_or("");
    auto root = path::FindSMRoot(dest);

    if (root)
    {
        GetContext().root = root.value();
    }
    else
    {
        Fail("SourceMod root not found.");
    }
}

LoadAddons::LoadAddons(Logger& logger, CommonContext& context,
                       std::filesystem::path path) noexcept
    : Operation(logger, context), path(std::move(path))
{
}

void LoadAddons::Run() noexcept
{
    if (!path::HasReadAndWritePermissions(path))
    {
        Fail("No read or write permissions.");
    }

    if (!Addon::Load(path))
    {
        Fail("Failed to load installed addons.");
    }
}

SaveAddons::SaveAddons(Logger& logger, CommonContext& context,
                       std::filesystem::path path) noexcept
    : Operation(logger, context), path(std::move(path))
{
}

void SaveAddons::Run() noexcept
{
    if (!path::HasReadAndWritePermissions(path))
    {
        Fail("No read or write permissions.");
    }

    if (!Addon::Save(path))
    {
        Fail("Failed to save installed addons.");
    }
}
}  // namespace smam
