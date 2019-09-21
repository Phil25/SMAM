#include "installer.h"

namespace smam
{
InstallerContext::InstallerContext(
    const std::string&              databaseUrl,
    const std::vector<std::string>& ids) noexcept
    : database(databaseUrl, ids)
{
}

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
