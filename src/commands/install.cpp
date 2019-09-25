#include "common.h"

#include <operations/installer.h>

namespace smam
{
auto command::Install(Logger& logger, const Options& options) noexcept
    -> ExitCode
{
    auto exec = Executor<InstallerContext>(logger);
    auto db   = options.DatabaseUrl();
    auto ids  = options.Addons();

    auto setupError = exec.Run<PrecacheAddons>(db, ids)
                          .Run<InitScrapers>()
                          .GetError();

    if (setupError)
    {
        logger << setupError.message << cr;
        return ExitCode::Failure;
    }

    for (const auto& id : ids)
    {
        auto error = exec.Run<CheckPending>(id)
                         .Run<SetAddon>(id)
                         .Run<CheckInstalled>(options.Force())
                         .Run<InstallDependencies>()
                         .Run<InstallAddon>()
                         .GetError();

        if (error)
        {
            logger << error.message << cr;
        }
    }

    return ExitCode::OK;
}
}  // namespace smam
