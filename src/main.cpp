#include <iostream>

#include "opts.hpp"
#include "installer.h"
#include "downloader.h"

constexpr char cr = '\n';
constexpr char tab = '\t';

void install(const Opts& opts)
{
	Downloader down;
	Database db(down);
	Installer::initScrapers(down);

	const auto& addons = opts.getAddons();
	db.precache(addons);

	for(const auto& addon : addons)
	{
		std::cout << "Installing " << addon << "..." << cr;
		for(const auto& file : Installer::getFiles(addon, db))
		{
			std::cout << tab << "    file : " << file.name << cr;
			std::cout << tab << "get from : " << file.url << cr;
		}
	}
}

void remove(const Opts&)
{
}

void info(const Opts&)
{
}

void search(const Opts&)
{
}

int main(int argc, const char* argv[])
{
	const Opts opts(argc, argv);

	if(opts.help())
	{
		std::cout << opts.getDescription() << '\n';
	}
	else
	{
		const auto& command = opts.getCommand();

		if(opts.quiet())
		{
			std::cout.rdbuf(nullptr); // mute std::cout
		}

		try
		{
			std::map<std::string_view, void (*)(const Opts&)>
			{
				{"install", install},
				{"remove", remove},
				{"uninstall", remove},
				{"info", info},
				{"search", search},
			}
			.at(command)(opts);
		}
		catch(const std::out_of_range& e)
		{
			std::cerr << "error: unknown command \"" << command << "\"\n";
			return 1;
		}
	}

	return 0;
}
