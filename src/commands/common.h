#pragma once

#include <opts.h>

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

namespace Command
{
using fptr = ExitCode (*)(const Opts&) noexcept;
extern const std::map<std::string_view, fptr> map;

auto install(const Opts&) noexcept -> ExitCode;
auto remove(const Opts&) noexcept -> ExitCode;
auto info(const Opts&) noexcept -> ExitCode;
auto search(const Opts&) noexcept -> ExitCode;

bool exists(const std::string& command) noexcept;
auto run(const std::string& command, const Opts& opts) noexcept
    -> ExitCode;

}  // namespace Command

namespace Common
{
[[nodiscard]] bool noAddons(const std::vector<std::string>&) noexcept;
[[nodiscard]] bool noSMRoot(const Opts& opts) noexcept;
[[nodiscard]] auto load() noexcept -> ExitCode;
[[nodiscard]] bool save() noexcept;
}  // namespace Common
