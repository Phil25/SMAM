#pragma once

#include <utils/logger.h>

#include <boost/program_options.hpp>

namespace smam
{
class Options final
{
    bool failed{false};

    boost::program_options::options_description helpDesc;
    boost::program_options::variables_map       vm;

public:
    Options(int argc, const char* argv[], const LoggerPtr&) noexcept;

    bool InFailedState() const;
    auto GenHelp(const char* binary) const noexcept -> std::string;

    bool Help() const noexcept;
    bool Version() const noexcept;
    bool Quiet() const noexcept;
    bool Force() const noexcept;
    bool NoDeps() const noexcept;
    bool NoColor() const noexcept;
    bool NoPrefix() const noexcept;
    bool AllowRoot() const noexcept;

    auto Command() const noexcept -> const std::string&;
    auto Addons() const noexcept -> const std::vector<std::string>&;
    auto DatabaseUrl() const noexcept -> std::string;
    auto LogFile() const noexcept -> std::optional<std::string>;
    auto Destination() const noexcept -> std::optional<std::string>;
};

using OptionsPtr = std::shared_ptr<Options>;
}  // namespace smam
