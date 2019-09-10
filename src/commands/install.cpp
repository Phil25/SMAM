#include "common.h"

#include <operations/addon.h>

namespace smam
{
auto command::Install(Logger& logger, const Options& options) noexcept
    -> ExitCode
{
    auto exec  = Executor<AddonContext>(logger);
    auto error = exec.Run<CheckPending>("id").GetError();

    if (error)
    {
        logger << error.message << cr;
    }

    return ExitCode::OK;
}
}  // namespace smam
