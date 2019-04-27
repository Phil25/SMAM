#include <iostream>

#include "opts.hpp"

#include "utils/archive.h"
#include "installer.h"
#include "downloader.h"

constexpr char cr = '\n';
constexpr char tab = '\t';

using execCmd = int (*)(const Opts&);
namespace fs = std::filesystem;

bool goToSMRoot(const fs::path& dest)
{
	if(!dest.empty())
	{
		fs::current_path(dest);
	}

	if(fs::is_directory("./plugins"))
	{
		return true; // already at SM root
	}

	for(const auto& prev : {"./sourcemod", "./addons/sourcemod"})
	{
		if(fs::is_directory(prev))
		{
			fs::current_path(prev);
			return true;
		}
	}

	return false;
}

int install(const Opts& opts)
{
	if(!goToSMRoot(opts.destination().value_or("")))
	{
		std::cerr << "error: could not find SourceMod root" << cr;
		return 1;
	}

	Downloader down;
	Database db(down, opts.getDbUrl());
	Installer::initScrapers(down);

	const auto& addons = opts.getAddons();
	db.precache(addons);

	for(const auto& addon : addons)
	{
		std::cout << "Installing " << addon << "..." << cr;
		for(const auto& file : Installer::getFiles(addon, db))
		{
			fs::path path = file.path;
			fs::create_directories(file.path);
			path.append(file.name);

			down.file(file.url, path);

			if(Archive::valid(path)) Archive::extract(path);
		}
	}

	return 0;
}

int remove(const Opts&)
{
	return 0;
}

int info(const Opts&)
{
	return 0;
}

int search(const Opts&)
{
	return 0;
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
			return std::map<std::string_view, execCmd>
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
