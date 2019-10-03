#include "installer.h"

#include <net/database.h>
#include <operations/addon.h>

namespace smam
{
InstallerContext::InstallerContext(std::string id,
                                   AddonMap    cache) noexcept
    : id(std::move(id)), cache(std::move(cache))
{
}

CheckPending::CheckPending(Logger& l, InstallerContext& c) noexcept
    : Operation(l, c)
{
}

void CheckPending::Run() noexcept
{
    if (GetContext().pendingToBeInstalled.count(GetContext().id))
    {
        Stop();
    }

    GetContext().pendingToBeInstalled.insert(GetContext().id);
}

ParseCache::ParseCache(Logger& l, InstallerContext& c) noexcept
    : Operation(l, c)
{
}

void ParseCache::Run() noexcept
{
    const auto& cache = GetContext().cache;

    if (auto it = cache.find(GetContext().id); it != cache.end())
    {
        GetContext().addon = it->second;
    }
    else
    {
        Fail("Not found: \"" + GetContext().id + '"');
    }
}

MarkExplicit::MarkExplicit(Logger& l, InstallerContext& c) noexcept
    : Operation(l, c)
{
}

void MarkExplicit::Run() noexcept
{
    assert(GetContext().addon);

    /*
     * Every user-specified addon should be marked as explicit.
     * Needs to be marked before CheckInstalled in case the user wants
     * to specify explicitness, after the addon has potentially
     * been installed as a dependnecy initially.
     * TODO: Notify the user about marking an addon as explicit.
     */
    GetContext().addon->MarkExplicit();
}

CheckInstalled::CheckInstalled(Logger& l, InstallerContext& c,
                               bool force) noexcept
    : Operation(l, c), force(force)
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
    Logger& logger, InstallerContext& context,
    const std::shared_ptr<ScraperArray>& scrapers) noexcept
    : Operation(logger, context), scrapers(scrapers)
{
}

void InstallDependencies::Run() noexcept
{
    assert(GetContext().addon);

    for (const auto& dep : GetContext().addon->Dependencies())
    {
        // TODO: finish me after figuring out installation
    }
}

InstallAddon::InstallAddon(
    Logger& logger, InstallerContext& context,
    const std::shared_ptr<ScraperArray>& scrapers) noexcept
    : Operation(logger, context), scrapers(scrapers)
{
}

void InstallAddon::Run() noexcept
{
    assert(GetContext().addon);

    auto error = Executor<AddonContext>(GetLogger(), GetContext().addon)
                     .Run<FindData>(scrapers)
                     .Run<EvaluateFiles>()
                     .Run<DownloadFiles>()
                     .Run<MarkInstalled>()
                     .GetError();

    if (error) Fail(error.message);
}
}  // namespace smam
