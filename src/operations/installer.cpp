#include "installer.h"

#include <net/database.h>
#include <operations/addon.h>

#include <scrapers/amscraper.h>
#include <scrapers/ghscraper.h>
#include <scrapers/ltscraper.h>

namespace smam
{
PrecacheAddons::PrecacheAddons(
    Logger& logger, InstallerContext& context,
    const std::string&              databaseUrl,
    const std::vector<std::string>& ids) noexcept
    : Operation(logger, context), databaseUrl(databaseUrl), ids(ids)
{
}

void PrecacheAddons::Run() noexcept
{
    auto& data = GetContext().data;

    data = Database(GetLogger(), databaseUrl, ids).Cached();

    for (const auto& id : ids)
    {
        if (auto it = data.find(id); it != data.end())
        {
            // Mark every user-specified addon as explicit.
            it->second.addon->MarkExplicit();
        }
    }
}

InitScrapers::InitScrapers(Logger&           logger,
                           InstallerContext& context) noexcept
    : Operation(logger, context)
{
}

void InitScrapers::Run() noexcept
{
    GetContext().scrapers[0] = std::make_unique<AMScraper>();
    GetContext().scrapers[1] = std::make_unique<LTScraper>();
    GetContext().scrapers[2] = std::make_unique<GHScraper>();
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

    GetContext().pendingToBeInstalled.emplace(std::move(currentID));
}

SetAddon::SetAddon(Logger& logger, InstallerContext& context,
                   std::string currentID) noexcept
    : Operation(logger, context), currentID(std::move(currentID))
{
}

void SetAddon::Run() noexcept
{
    const auto& data = GetContext().data;

    if (auto it = data.find(currentID); it == data.end())
    {
        Fail("Not found: \"" + currentID + '"');
    }
    else
    {
        GetContext().addon = it->second.addon;
        GetContext().url   = std::move(it->second.url);
    }
}

CheckInstalled::CheckInstalled(Logger&           logger,
                               InstallerContext& context,
                               bool              force) noexcept
    : Operation(logger, context), force(force)
{
}

void CheckInstalled::Run() noexcept
{
    if (force) return;  // skip this check

    const auto& addon = GetContext().addon;
    assert(addon);

    if (addon->IsInstalled())
    {
        Fail("Already installed: \"" + addon->ID() + '"');
    }
}

InstallDependencies::InstallDependencies(
    Logger& logger, InstallerContext& context) noexcept
    : Operation(logger, context)
{
}

void InstallDependencies::Run() noexcept
{
    for (const auto& dep : GetContext().addon->Dependencies())
    {
        // TODO: finish me after figuring out installation
    }
}

InstallAddon::InstallAddon(Logger&           logger,
                           InstallerContext& context) noexcept
    : Operation(logger, context)
{
}

void InstallAddon::Run() noexcept
{
    const auto& con = GetContext();

    auto exec  = Executor<AddonContext>(GetLogger());
    auto error = exec.Run<InitAddonContext>(con.addon)
                     .Run<FindData>(std::cref(con.scrapers), con.url)
                     .Run<EvaluateFiles>()
                     .Run<DownloadFiles>()
                     .Run<MarkInstalled>()
                     .GetError();

    if (error)
    {
        Fail(error.message);
    }
}
}  // namespace smam
