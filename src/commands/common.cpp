#include "common.h"

const std::map<std::string_view, Command::fptr> Command::map{
    {"install", Command::install},  {"remove", Command::remove},
    {"uninstall", Command::remove}, {"info", Command::info},
    {"list", Command::info},        {"search", Command::search},
};

bool Command::exists(const std::string& command) noexcept
{
    return map.count(command);
}

auto Command::run(const std::string& command, const Opts& opts) noexcept
    -> ExitCode
{
    return map.at(command)(opts);
}
