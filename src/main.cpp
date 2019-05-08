#include "opts.hpp"
#include "version.hpp"

#include "utils/archive.h"
#include "utils/printer.h"

#include "installer.h"
#include "downloader.h"

using execCmd = int (*)(const Opts&);
namespace fs = std::filesystem;

bool goToSMRoot(const fs::path& startAt)
{
	if(!startAt.empty())
	{
		fs::current_path(startAt);
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
		out(Ch::Error) << "Could not find SourceMod root." << cr;
		return 1;
	}

	Downloader down;
	Database db(down, opts.getDbUrl());
	Installer::initScrapers(down);

	const auto& addons = opts.getAddons();
	db.precache(addons);

	for(const auto& addon : addons)
	{
		out(Ch::Info)
			<< Col::green
			<< "Installing " << addon << "..."
			<< Col::reset << cr;

		for(const auto& file : Installer::getFiles(addon, db))
		{
			fs::path path = file.path;
			fs::create_directories(file.path);
			path.append(file.name);

			out() << path << cr;

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
		opts.printHelp(argv[0], out.getStream());
		return 0;
	}

	if(opts.version())
	{
		out.noPrefix();
		out() << Version::full() << cr;
		return 0;
	}

	if(opts.quiet())	out.quiet();
	if(opts.noPrefix())	out.noPrefix();
	if(opts.noColor())	out.colors = false;

	if(argc == 1)
	{
		out(Ch::Error) << "No command provided." << cr;
		return 1;
	}

	const auto& command = opts.getCommand();
	const std::map<std::string_view, execCmd> cmdMap{
		{"install",		install},
		{"remove",		remove},
		{"uninstall",	remove},
		{"info",		info},
		{"search",		search},
	};

	if(!cmdMap.count(command))
	{
		out(Ch::Error) << "Unknown command: \"" << command << '\"' << cr;
		return 1;
	}

	if(argc == 2)
	{
		out(Ch::Error) << "No addons specified." << cr;
		return 1;
	}

	cmdMap.at(command)(opts);
	return 0;
}
