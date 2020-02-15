#include "common.h"

#include <operations/common.h>
#include <data/addon.h>

namespace smam
{
auto command::Info(const LoggerPtr&  logger,
                   const OptionsPtr& options) noexcept -> ExitCode
{
    auto error = Executor<CommonContext>(logger, options)
                     .Run<GoToSMRoot>()
                     .Run<LoadAddons>(".smamdata.json")
                     .GetError();

    if (error)
    {
        logger->Error() << error.message << cr;
        return error.code;
    }

    const auto& filter = options->Addons();

    if (filter.empty())
    {
        logger->Out() << "Installed addons:" << cr;
        Addon::ForEach([&logger](AddonPtr addon) {
            logger->Out() << addon->ID() << cr;
        });

        return ExitCode::OK;
    }

    for (const auto& id : filter)
    {
        if (!Addon::IsInstalled(id))
        {
            logger->Warning()
                << "Addon " << Col::green << id << Col::reset
                << " is not installed." << cr;
            continue;
        }

        const auto addon = Addon::Get(id).value();

        logger->Out() << "ID: " << Col::green << id << Col::reset << cr;
        logger->Out() << "Author: " << addon->Author() << cr;
        logger->Out() << "Description: " << addon->Description() << cr;

        logger->Out() << "Files: [" << cr;
        for (const auto& file : addon->Files())
        {
            logger->Out() << '\t' << file->Path() << '/' << file->Name()
                          << ',' << cr;
        }
        logger->Out() << "]" << cr;

        logger->Out() << "Dependencies: [" << cr;
        for (const auto& dep : addon->Dependencies())
        {
            logger->Out() << '\t' << dep << ',' << cr;
        }
        logger->Out() << "]" << cr;
    }

    return ExitCode::OK;
}
}  // namespace smam
