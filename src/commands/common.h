#pragma once

#include <utils/options.h>

namespace smam
{
enum ExitCode
{
    OK        = 0,
    RunAsRoot = 1,
    NoPermissions,
    WriteError,
    NoCommand,
    UnknownCommand,
    NoAddons,
    NoSMRoot,
    CorruptedCache,
};

namespace command
{
using CommandPtr = ExitCode (*)(Logger&, const Options&) noexcept;
using CommandMap = std::map<std::string_view, CommandPtr>;
extern const CommandMap map;

auto Install(Logger&, const Options&) noexcept -> ExitCode;
auto Remove(Logger&, const Options&) noexcept -> ExitCode;
auto Info(Logger&, const Options&) noexcept -> ExitCode;
auto Search(Logger&, const Options&) noexcept -> ExitCode;

bool Exists(const std::string& command) noexcept;
auto Run(Logger&, const std::string& command, const Options&) noexcept
    -> ExitCode;
}  // namespace command
}  // namespace smam
