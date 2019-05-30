#include <boost/program_options.hpp>
#include <iomanip>

#include "utils/printer.h"

class Opts
{
    boost::program_options::options_description helpDesc;
    boost::program_options::variables_map       vm = {};

public:
    Opts(int argc, const char* argv[]) noexcept
    {
        namespace po = boost::program_options;

        try
        {
            po::options_description general("Options");

            // clang-format off
            general.add_options()
                ("help,h", "Show help.")
                ("version", "Show version.")
                ("quiet,q", "Do not produce output.")
                ("force,f", "Force command execution.")
                ("no-prefix", "Disable prefixes in output.")
                ("no-color", "Disable color in output.")
                ("force-running-as-root", "Run SMAM even as root.")
                ("log", po::value<std::string>(), "Path to log file.")
                ("destination,d", po::value<std::string>(), "Path to server.")
                ("db-url", po::value<std::string>()
                    ->default_value("https://smamdb.net/"),
                    "URL of the database.");

            po::options_description hidden("Hidden");
            hidden.add_options()
                ("command", po::value<std::string>()
                    ->default_value("")->required(), "")
                ("addons", po::value<std::vector<std::string>>()
                    ->default_value({}, "")->required(), "");
            // clang-format on

            helpDesc.add(general);
            general.add(hidden);

            po::positional_options_description pDesc;
            pDesc.add("command", 1);
            pDesc.add("addons", -1);

            po::command_line_parser parser{argc, argv};
            parser.options(general)
                .positional(pDesc)
                .allow_unregistered();
            store(parser.run(), vm);
        }
        catch (const po::error& e)
        {
            out(Ch::Error) << e.what() << cr;
        }
    }

    void printHelp(const char* bin, Printer& p) const noexcept
    {
        using sv_pair = std::pair<std::string_view, std::string_view>;
        constexpr std::array<sv_pair, 4> commands{
            sv_pair("  install", "Install specified addons."),
            sv_pair("  remove", "Remove specified addons."),
            sv_pair("  search", "Search for addons."),
            sv_pair("  info", "Get info about installed addons."),
        };
        const auto offset = helpDesc.get_option_column_width();

        p << cr << "Usage:" << cr;
        p << "  " << bin << " <command> [addons] [options]" << cr;

        p << cr << "Commands:" << cr;
        for (auto [cmd, desc] : commands)
        {
            auto width = offset - cmd.size() + desc.size();
            p << cmd << std::setw(width) << desc << cr;
        }

        p << helpDesc;
    }

    const auto& getCommand() const noexcept
    {
        return vm["command"].as<std::string>();
    }

    const auto& getAddons() const noexcept
    {
        return vm["addons"].as<std::vector<std::string>>();
    }

    const auto& getDbUrl() const noexcept
    {
        return vm["db-url"].as<std::string>();
    }

    bool help() const noexcept { return vm.count("help"); }

    bool version() const noexcept { return vm.count("version"); }

    bool quiet() const noexcept { return vm.count("quiet"); }

    bool force() const noexcept { return vm.count("force"); }

    bool noColor() const noexcept { return vm.count("no-color"); }

    bool noPrefix() const noexcept { return vm.count("no-prefix"); }

    bool forceRoot() const noexcept
    {
        return vm.count("force-running-as-root");
    }

    auto log() const noexcept -> std::optional<std::string>
    {
        if (vm.count("log"))
        {
            return vm["log"].as<std::string>();
        }
        else
        {
            return std::nullopt;
        }
    }

    auto destination() const noexcept -> std::optional<std::string>
    {
        if (vm.count("destination"))
        {
            return vm["destination"].as<std::string>();
        }
        else
        {
            return std::nullopt;
        }
    }
};
