#include "installer.h"

namespace smam
{
InitializeScrapers::InitializeScrapers(
    Logger& logger, InstallerContext& context) noexcept
    : Operation(logger, context)
{
}

void InitializeScrapers::Run() noexcept
{
}

CheckPending::CheckPending(Logger& logger, InstallerContext& context,
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
