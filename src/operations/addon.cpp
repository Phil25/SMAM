#include "addon.h"

namespace smam
{
CheckPending::CheckPending(Logger& logger, AddonContext& context,
                           std::string currentID) noexcept
    : Operation(logger, context), currentID(std::move(currentID))
{
}

void CheckPending::Run() noexcept
{
    if (GetContext().pendingToBeInstalled.count(currentID))
    {
        Stop();
    }
}
}  // namespace smam
