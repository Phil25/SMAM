#include "common.h"

namespace smam
{
const command::CommandMap command::map = {
    {"install", command::Install},  {"remove", command::Remove},
    {"uninstall", command::Remove}, {"info", command::Info},
    {"list", command::Info},        {"search", command::Search}};

bool command::Exists(const std::string& command) noexcept
{
    return map.count(command);
}

auto command::Run(const LoggerPtr& logger, const std::string& command,
                  const OptionsPtr& options) noexcept -> ExitCode
{
    return map.at(command)(logger, options);
}
}  // namespace smam
