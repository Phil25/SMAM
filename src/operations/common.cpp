#include "common.h"

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
}  // namespace smam
