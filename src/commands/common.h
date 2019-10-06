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
using CommandPtr = ExitCode (*)(const LoggerPtr&,
                                const OptionsPtr&) noexcept;

using CommandMap = std::map<std::string_view, CommandPtr>;
extern const CommandMap map;

auto Install(const LoggerPtr&, const OptionsPtr&) noexcept -> ExitCode;
auto Remove(const LoggerPtr&, const OptionsPtr&) noexcept -> ExitCode;
auto Info(const LoggerPtr&, const OptionsPtr&) noexcept -> ExitCode;
auto Search(const LoggerPtr&, const OptionsPtr&) noexcept -> ExitCode;

bool Exists(const std::string& command) noexcept;
auto Run(const LoggerPtr&, const std::string& command,
         const OptionsPtr&) noexcept -> ExitCode;
}  // namespace command
}  // namespace smam
