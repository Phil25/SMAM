#include "remover.h"

#include <utils/path.h>

namespace
{
void RemoveAddon(smam::AddonPtr addon, smam::LoggerPtr logger) noexcept
{
    using namespace smam;
    namespace fs = std::filesystem;

    for (const auto& file : addon->Files())
    {
        auto count = Addon::CountByOwnedFile(file);
        assert(count && "File owned but not found");
        auto path = fs::path{file->Raw()};

        if (count > 1)
        {
            logger->Warning() << "Skipping shared file: " << path << cr;
            continue;
        }

        logger->Print() << path << cr;
        fs::remove(path);
        path::RemoveEmptyDirectories(path);
    }

    addon->MarkUninstalled();
    logger->Info() << "Removed " << Col::green << addon->ID()
                   << Col::reset << cr;
}
}  // namespace

namespace smam
{
RemoverContext::RemoverContext(std::string id) noexcept
    : id(std::move(id))
{
}

CheckNotInstalled::CheckNotInstalled(const LoggerPtr& logger,
                                     RemoverContext&  context) noexcept
    : Operation(logger, context)
{
}

void CheckNotInstalled::Run() noexcept
{
    const auto& id = GetContext().id;

    if (auto addonOpt = Addon::Get(id))
    {
        GetContext().addon = addonOpt.value();
        GetLogger()->Info() << "Removing " << Col::green << id
                            << Col::reset << "..." << cr;
    }
    else
    {
        Fail("Not installed: \"" + id + '"');
    }
}

RemoveAddon::RemoveAddon(const LoggerPtr& logger,
                         RemoverContext&  context) noexcept
    : Operation(logger, context)
{
}

void RemoveAddon::Run() noexcept
{
    assert(GetContext().addon);
    ::RemoveAddon(GetContext().addon, GetLogger());
}

RemoveDependencies::RemoveDependencies(const LoggerPtr& logger,
                                       RemoverContext&  context,
                                       bool             noDeps) noexcept
    : Operation(logger, context), noDeps(noDeps)
{
}

void RemoveDependencies::Run() noexcept
{
    assert(GetContext().addon);

    const auto remove = [&](const auto remove,
                            const auto addon) -> void {
        GetLogger()->Debug("Removing addon dependencies. ",
                           VAR(addon->ID()), ", ",
                           VAR(addon->Dependencies()));

        for (const auto& id : addon->Dependencies())
        {
            if (!Addon::Get(id).has_value()) continue;
            const auto dependency = Addon::Get(id).value();

            GetLogger()->Debug("Checking dependency... ", VAR(id));

            if (dependency->IsExplicit())
            {
                GetLogger()->Debug("Omitting explicit dependency.");
                continue;
            }

            dependency->MarkForRemoval();
            GetLogger()->Debug("Marking dependency for removal");

            GetLogger()->Debug(VAR(Addon::CountByDependency(id)));
            if (Addon::CountByDependency(id) > 0)  // addons left
            {
                GetLogger()->Debug("Omitting shared dependency.");
                remove(remove, dependency);  // still continue
                continue;
            }

            if (noDeps)
            {
                GetLogger()->Warning()
                    << "Loose dependency " << Col::green
                    << dependency->ID() << Col::reset << " not removed!"
                    << cr;
                continue;
            }

            GetLogger()->Info()
                << "Removing loose dependency: " << Col::green
                << dependency->ID() << Col::reset << "..." << cr;

            ::RemoveAddon(dependency, GetLogger());
            remove(remove, dependency);
        }
    };

    remove(remove, GetContext().addon);
}
}  // namespace smam
