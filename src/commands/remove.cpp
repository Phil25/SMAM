#include "common.h"

#include <operations/common.h>
#include <operations/remover.h>

namespace smam
{
auto command::Remove(const LoggerPtr&  logger,
                     const OptionsPtr& options) noexcept -> ExitCode
{
    auto exec  = Executor<CommonContext>(logger, options);
    auto error = exec.Run<CheckAddons>()
                     .Run<GoToSMRoot>()
                     .Run<LoadAddons>(".smamdata.json")
                     .GetError();

    if (error)
    {
        logger->Error() << error.message << cr;
        return error.code;
    }

    for (const auto& id : options->Addons())
    {
        error = Executor<RemoverContext>(logger, id)
                    .Run<CheckNotInstalled>()
                    .Run<RemoveAddon>()
                    .Run<RemoveDependencies>(options->NoDeps())
                    .GetError();

        if (error)
        {
            logger->Error() << error.message << cr;
        }
    }

    error = exec.Run<SaveAddons>(".smamdata.json").GetError();

    if (error)
    {
        logger->Error() << error.message << cr;
        return error.code;
    }

    return ExitCode::OK;
}
}  // namespace smam
