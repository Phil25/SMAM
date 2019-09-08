#include "common.h"

namespace smam
{
auto command::Remove(Logger& logger, const Options& options) noexcept
    -> ExitCode
{
    return ExitCode::OK;
}
}  // namespace smam
