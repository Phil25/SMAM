#pragma once

#include <utils/options.h>
#include <utils/codes.hpp>

namespace smam::command
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
}  // namespace smam::command
