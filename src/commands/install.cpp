#include "common.h"

namespace smam
{
auto command::Install(Logger& logger, const Options& options) noexcept
    -> ExitCode
{
    return ExitCode::OK;
}
}  // namespace smam
