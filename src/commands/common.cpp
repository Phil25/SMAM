#include "common.h"

namespace smam::command
{
const CommandMap map = {};

bool Exists(const std::string& command) noexcept
{
    return map.count(command);
}

auto Run(const std::string& command, const Options& options) noexcept
    -> ExitCode
{
    return map.at(command)(options);
}
}  // namespace smam::command
