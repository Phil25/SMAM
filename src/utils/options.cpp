#include "options.h"

#include <iomanip>

namespace smam
{
Options::Options(int argc, const char* argv[], Logger logger) noexcept
    : logger(std::move(logger))
{
    namespace po = boost::program_options;

    try
    {
        po::options_description general("Options");

        // clang-format off
        general.add_options()
            ("help,h", "Show help.")
            ("version,v", "Show version.")
            ("quiet,q", "Do not produce output.")
            ("force,f", "Force command execution.")
            ("no-deps", "Do not install/remove depenencies.")
            ("no-prefix", "Disable prefixes in output.")
            ("no-color", "Disable color in output.")
            ("allow-running-as-root", "Run SMAM even as root.")
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
        parser.options(general).positional(pDesc).allow_unregistered();
        store(parser.run(), vm);
    }
    catch (const po::error& e)
    {
        logger.Error() << e.what() << cr;
    }
}

auto Options::GenHelp(const char* binary) const noexcept -> std::string
{
    using sv_pair = std::pair<std::string_view, std::string_view>;
    std::ostringstream oss;

    constexpr std::array<sv_pair, 4> commands{
        sv_pair("  install", "Install specified addons."),
        sv_pair("  remove", "Remove specified addons."),
        sv_pair("  search", "Search for addons."),
        sv_pair("  info", "Get info about installed addons."),
    };

    const auto offset = helpDesc.get_option_column_width();

    oss << cr << "Usage:" << cr;
    oss << "  " << binary << " <command> [addons] [options]" << cr;

    oss << cr << "Commands:" << cr;
    for (auto [cmd, desc] : commands)
    {
        auto width = offset - cmd.size() + desc.size();
        oss << cmd << std::setw(width) << desc << cr;
    }

    oss << helpDesc;
    return oss.str();
}

bool Options::Help() const noexcept
{
    return vm.count("help");
}

bool Options::Version() const noexcept
{
    return vm.count("version");
}

bool Options::Quiet() const noexcept
{
    return vm.count("quiet");
}

bool Options::Force() const noexcept
{
    return vm.count("force");
}

bool Options::NoDeps() const noexcept
{
    return vm.count("no-deps");
}

bool Options::NoColor() const noexcept
{
    return vm.count("no-color");
}

bool Options::NoPrefix() const noexcept
{
    return vm.count("no-prefix");
}

bool Options::AllowRoot() const noexcept
{
    return vm.count("allow-running-as-root");
}

auto Options::Command() const noexcept -> const std::string&
{
    return vm["command"].as<std::string>();
}

auto Options::Addons() const noexcept -> const std::vector<std::string>&
{
    return vm["addons"].as<std::vector<std::string>>();
}

auto Options::DatabaseUrl() const noexcept -> const std::string&
{
    return vm["db-url"].as<std::string>();
}

auto Options::LogFile() const noexcept -> std::optional<std::string>
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

auto Options::Destination() const noexcept -> std::optional<std::string>
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
}  // namespace smam
