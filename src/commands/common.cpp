#include "common.h"

namespace smam::command
{
const CommandMap map = {
    {"install", command::Install},  {"remove", command::Remove},
    {"uninstall", command::Remove}, {"info", command::Info},
    {"list", command::Info},        {"search", command::Search}};

bool Exists(const std::string& command) noexcept
{
    return map.count(command);
}

auto Run(Logger& logger, const std::string& command,
         const Options& options) noexcept -> ExitCode
{
    return map.at(command)(logger, options);
}
}  // namespace smam::command
