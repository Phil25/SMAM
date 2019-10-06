#include "common.h"

namespace smam
{
auto command::Info(const LoggerPtr&  logger,
                   const OptionsPtr& options) noexcept -> ExitCode
{
    return ExitCode::OK;
}
}  // namespace smam
