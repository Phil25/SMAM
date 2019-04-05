#include <boost/program_options.hpp>
#include <functional>
#include <iostream>

class Opts
{
	boost::program_options::options_description desc;
	boost::program_options::variables_map vm;

public:
	Opts(int argc, const char* argv[]):
		desc("Options"),
		vm({})
	{
		namespace po = boost::program_options;

		try
		{
			desc.add_options()
				("help,h", "Show help.")
				("quiet,q", "Do not produce output.")
				("no-color", "Disable color in output")
				("log", po::value<std::string>(), "Path to log file.")
				("destination", po::value<std::string>(), "Path to server.")
				("command", po::value<std::string>()
					->default_value("")->required(), "")
				("addons", po::value<std::vector<std::string>>()
					->default_value({}, "")->required(), "");

			po::positional_options_description pDesc;
			pDesc.add("command", 1);
			pDesc.add("addons", -1);

			po::command_line_parser parser{argc, argv};
			parser.options(desc).positional(pDesc).allow_unregistered();
			store(parser.run(), vm);
		}
		catch(const po::error& e)
		{
			std::cerr << "error: " << e.what() << '\n';
		}
	}

	const auto& getDescription() const
	{
		return desc;
	}

	const auto& getCommand() const
	{
		return vm["command"].as<std::string>();
	}

	const auto& getAddons() const
	{
		return vm["addons"].as<std::vector<std::string>>();
	}

	bool help() const
	{
		return vm.count("help");
	}

	bool quiet() const
	{
		return vm.count("quiet");
	}

	bool noColor() const
	{
		return vm.count("no-color");
	}

	auto log() const -> std::optional<std::string>
	{
		if(vm.count("log"))
		{
			return vm["log"].as<std::string>();
		}
		else
		{
			return std::nullopt;
		}
	}

	auto destination() const -> std::optional<std::string>
	{
		if(vm.count("destination"))
		{
			return vm["destination"].as<std::string>();
		}
		else
		{
			return std::nullopt;
		}
	}
};
