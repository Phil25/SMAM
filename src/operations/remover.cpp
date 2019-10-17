#include "remover.h"

#include <utils/path.h>

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
    namespace fs = std::filesystem;

    const auto& addon = GetContext().addon;
    assert(addon);

    for (const auto& file : addon->Files())
    {
        auto count = Addon::CountByOwnedFile(file);
        assert(count && "File owned but not found");
        auto path = fs::path{file->Raw()};

        if (count > 1)
        {
            GetLogger()->Warning()
                << "Skipping shared file: " << path << cr;
            continue;
        }

        GetLogger()->Info() << path << cr;
        fs::remove(path);
        path::RemoveEmptyDirectories(path);
    }

    addon->MarkUninstalled();
}

RemoveDependencies::RemoveDependencies(const LoggerPtr& logger,
                                       RemoverContext& context) noexcept
    : Operation(logger, context)
{
}

void RemoveDependencies::Run() noexcept
{
    const auto& addon = GetContext().addon;
    assert(addon);
}
}  // namespace smam
