#include "installer.h"

#include <net/database.h>
#include <operations/addon.h>

namespace smam
{
constexpr std::string_view tempdir  = "/tmp/smam/addons/sourcemod/";
constexpr std::string_view tempcore = "/tmp/smam/";

InstallerContext::InstallerContext(std::string        id,
                                   const AddonMapPtr& cache) noexcept
    : id(std::move(id)), cache(cache)
{
}

CheckPending::CheckPending(const LoggerPtr&  l,
                           InstallerContext& c) noexcept
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

ParseCache::ParseCache(const LoggerPtr& l, InstallerContext& c) noexcept
    : Operation(l, c)
{
}

void ParseCache::Run() noexcept
{
    const auto& cache = GetContext().cache;

    if (auto it = cache->find(GetContext().id); it != cache->end())
    {
        GetContext().addon = it->second;
    }
    else
    {
        Fail("Not found: \"" + GetContext().id + '"');
    }
}

MarkExplicit::MarkExplicit(const LoggerPtr&  l,
                           InstallerContext& c) noexcept
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
     * been installed as a dependency initially.
     * TODO: Notify the user about marking an addon as explicit.
     */

    if (const auto addonOpt = Addon::Get(GetContext().addon->ID()))
    {
        addonOpt.value()->MarkExplicit();  // mark installed addon
    }
    else
    {
        GetContext().addon->MarkExplicit();  // mark new addon
    }
}

CheckInstalled::CheckInstalled(const LoggerPtr& l, InstallerContext& c,
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
        return;
    }

    GetLogger()->Info() << "Installing " << Col::green << addon->ID()
                        << Col::reset << "..." << cr;
}

InstallDependencies::InstallDependencies(
    const LoggerPtr& logger, InstallerContext& context,
    const ScraperArrayPtr& scrapers) noexcept
    : Operation(logger, context), scrapers(scrapers)
{
}

void InstallDependencies::Run() noexcept
{
    assert(GetContext().addon);
    assert(GetContext().cache);

    // waiting until Y combinators are added to the standard in 10 years
    const auto install = [&](const auto install, const auto addon) {
        for (const auto& dep : addon->Dependencies())
        {
            if (GetContext().pendingToBeInstalled.count(dep))
            {
                continue;  // circular dependency prevention
            }

            GetContext().pendingToBeInstalled.insert(dep);

            if (!GetContext().cache->count(dep))
            {
                return Error{"Dependency " + dep + " not found."};
            }

            const auto dependency = GetContext().cache->at(dep);

            //  recursively install its own dependencies
            auto error = install(install, dependency);

            if (error) return error;

            error = Executor<AddonContext>(GetLogger(), dependency)
                        .Run<CheckSatisfied>()
                        .Run<FindData>(scrapers)
                        .Run<EvaluateFiles>()
                        .Run<DownloadFiles>()
                        .Run<ExtractArchives>()
                        .Run<MarkInstalled>()
                        .GetError();

            if (error) return error;
        };

        return Error{};
    };

    if (auto error = install(install, GetContext().addon); error)
    {
        Fail(error.message);
    }
}

InstallAddon::InstallAddon(const LoggerPtr&       logger,
                           InstallerContext&      context,
                           const ScraperArrayPtr& scrapers) noexcept
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
                     .Run<ExtractArchives>()
                     .Run<MarkInstalled>()
                     .GetError();

    if (error) Fail(error.message);
}

BeginTransaction::BeginTransaction(const LoggerPtr&  logger,
                                   InstallerContext& context) noexcept
    : Operation(logger, context)
{
}

void BeginTransaction::Run() noexcept
{
    namespace fs = std::filesystem;

    GetContext().root = fs::current_path();

    if (fs::exists(tempcore)) fs::remove_all(tempcore);

    if (!fs::create_directories(tempdir))
    {
        Fail("Could not initialize temporary directories.");
    }

    fs::current_path(tempdir);
}

CommitTransaction::CommitTransaction(const LoggerPtr&  logger,
                                     InstallerContext& context) noexcept
    : Operation(logger, context)
{
}

void CommitTransaction::Run() noexcept
{
    namespace fs = std::filesystem;
    using co     = fs::copy_options;

    fs::copy(tempdir, GetContext().root,
             co::recursive | co::overwrite_existing);

    fs::current_path(GetContext().root);
    fs::remove_all(tempcore);
}
}  // namespace smam
