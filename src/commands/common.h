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
using fptr       = ExitCode (*)(const Options&) noexcept;
using CommandMap = std::map<std::string_view, fptr>;
extern const CommandMap map;

bool Exists(const std::string& command) noexcept;
auto Run(const std::string& command, const Options&) noexcept
    -> ExitCode;
}  // namespace command
}  // namespace smam
