#include "installer.h"

#include <net/database.h>
#include <operations/addon.h>

namespace smam
{
constexpr std::string_view tempdir  = "/tmp/smam/addons/sourcemod/";
constexpr std::string_view tempcore = "/tmp/smam/";

InstallerContext::InstallerContext(std::string        id,
                                   const AddonMapPtr& metadata) noexcept
    : id(std::move(id)), metadata(metadata)
{
}

CheckPending::CheckPending(const LoggerPtr&  l,
                           InstallerContext& c) noexcept
    : Operation(l, c)
{
}

void CheckPending::Run() noexcept
{
    const auto& id = GetContext().id;

    if (GetContext().pendingToBeInstalled.count(id))
    {
        GetLogger()->Debug("Addon already pending. ", VAR(id));
        Stop();
    }

    GetLogger()->Debug("Adding addon to pending list. ", VAR(id));
    GetContext().pendingToBeInstalled.insert(id);
}

ParseMetadata::ParseMetadata(const LoggerPtr&  l,
                             InstallerContext& c) noexcept
    : Operation(l, c)
{
}

void ParseMetadata::Run() noexcept
{
    const auto& metadata = GetContext().metadata;
    const auto& id       = GetContext().id;

    if (auto it = metadata->find(id); it != metadata->end())
    {
        GetContext().addon = it->second;
        GetLogger()->Debug("ID found in metadata. ", VAR(id));
    }
    else
    {
        Fail("Not found: \"" + id + '"');
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

    const auto& id = GetContext().addon->ID();
    if (const auto addonOpt = Addon::Get(id))
    {
        GetLogger()->Debug("Marking installed addon as explicit. ",
                           VAR(id));
        addonOpt.value()->MarkExplicit();  // mark installed addon
    }
    else
    {
        GetLogger()->Debug(
            "Marking yet to be installed addon as explicit. ", VAR(id));
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
    if (force)
    {
        GetLogger()->Debug("Skipping CheckInstalled due to --force");
        return;  // skip this check
    }

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
    const ScraperArrayPtr& scrapers, bool noDeps) noexcept
    : Operation(logger, context), scrapers(scrapers), noDeps(noDeps)
{
}

void InstallDependencies::Run() noexcept
{
    assert(GetContext().addon);
    assert(GetContext().metadata);

    // waiting until Y combinators are added to the standard in 10 years
    const auto install = [&](const auto install, const auto addon) {
        const auto& addonId = addon->ID();
        const auto& deps    = addon->Dependencies();
        GetLogger()->Debug("Installing addon dependencies. ",
                           VAR(addonId), ", ", VAR(deps));

        for (const auto& dep : deps)
        {
            if (GetContext().pendingToBeInstalled.count(dep))
            {
                GetLogger()->Debug("Skipping circular dependency...");
                continue;  // circular dependency prevention
            }

            GetContext().pendingToBeInstalled.insert(dep);

            if (noDeps)
            {
                GetLogger()->Warning()
                    << "Dependency " << Col::green << dep << Col::reset
                    << " required but not installed!" << cr;
                continue;
            }

            if (!GetContext().metadata->count(dep))
            {
                return Error{"Dependency " + dep + " not found."};
            }

            const auto id = GetContext().metadata->at(dep);

            //  recursively install its own dependencies
            auto error = install(install, id);

            if (error) return error;

            error = Executor<AddonContext>(GetLogger(), id)
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
