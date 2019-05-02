#include "opts.hpp"

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

	if(opts.quiet())	out.quiet();
	if(opts.noPrefix())	out.noPrefix();
	if(opts.noColor())	out.colors = false;

	if(opts.help())
	{
		out() << opts.getDescription() << cr;
		return 0;
	}

	const auto& command = opts.getCommand();

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
		out(Ch::Error) << "Unknown command: \"" << command << '\"' << cr;
		return 1;
	}

	return 0;
}
