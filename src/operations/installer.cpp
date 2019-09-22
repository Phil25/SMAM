#include "installer.h"

#include <net/database.h>
#include <scrapers/scraper.h>

namespace smam
{
FetchData::FetchData(Logger& logger, InstallerContext& context,
                     const std::string&              databaseUrl,
                     const std::vector<std::string>& ids) noexcept
    : Operation(logger, context), databaseUrl(databaseUrl), ids(ids)
{
}

void FetchData::Run() noexcept
{
    auto& data   = GetContext().data;
    auto& logger = GetLogger();

    data = Database(logger, databaseUrl, ids).Cached();

    for (const auto& id : ids)
    {
        if (auto it = data.find(id); it != data.end())
        {
            // Mark every user-specified addon as explicit.
            it->second.addon->MarkExplicit();
        }
        else
        {
            logger.Error() << '"' << id << "\" not found." << cr;
        }
    }
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
