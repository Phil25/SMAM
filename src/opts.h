#pragma once

#include <utils/printer.h>

#include <boost/program_options.hpp>

class Opts final
{
    boost::program_options::options_description helpDesc;
    boost::program_options::variables_map       vm;

public:
    Opts(int argc, const char* argv[]) noexcept;

    void printHelp(const char* bin, Printer&) const noexcept;

    bool help() const noexcept;
    bool version() const noexcept;
    bool quiet() const noexcept;
    bool force() const noexcept;
    bool noDeps() const noexcept;
    bool noColor() const noexcept;
    bool noPrefix() const noexcept;
    bool allowRoot() const noexcept;

    auto getCommand() const noexcept -> const std::string&;
    auto getAddons() const noexcept -> const std::vector<std::string>&;
    auto getDbUrl() const noexcept -> const std::string&;
    auto getLog() const noexcept -> std::optional<std::string>;
    auto getDestination() const noexcept -> std::optional<std::string>;
};
