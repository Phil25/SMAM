#include "opts.hpp"
#include "version.hpp"

#include "utils/archive.h"
#include "utils/printer.h"
#include "utils/smfs.h"

#include "installer.h"
#include "downloader.h"

using execCmd = int (*)(const Opts&);
namespace fs = std::filesystem;

enum ExitCode
{
	OK = 0,
	WriteError = 1,
	NoCommand,
	UnknownCommand,
	NoAddons,
	NoSMRoot
};

int install(const Opts& opts)
{
	if(opts.getAddons().empty())
	{
		out(Ch::Error) << "No addons specified." << cr;
		return ExitCode::NoAddons;
	}

	auto root = SMFS::findRoot(opts.destination().value_or(""));

	if(root)
	{
		fs::current_path(root.value());
	}
	else
	{
		out(Ch::Error) << "SourceMod root not found." << cr;
		return ExitCode::NoSMRoot;
	}

	SMFS::loadData();
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

		for(const auto& f : Installer::getFiles(addon, db))
		{
			fs::path file = f.path;
			file.append(f.name);

			if(!SMFS::prepare(file.parent_path()))
			{
				out(Ch::Warn) << "Ignoring " << file << cr;
				continue;
			}

			if(!down.file(f.url, file))
			{
				continue;
			}

			out() << file << cr;

			if(Archive::valid(file))
			{
				SMFS::addFiles(addon, Archive::extract(file));
			}
			else
			{
				SMFS::addFile(addon, file);
			}
		}
	}

	return SMFS::writeData() ? ExitCode::OK : ExitCode::WriteError;
}

int remove(const Opts&)
{
	return ExitCode::OK;
}

int info(const Opts&)
{
	return ExitCode::OK;
}

int search(const Opts&)
{
	return ExitCode::OK;
}

int main(int argc, const char* argv[])
{
	const Opts opts(argc, argv);

	if(opts.help())
	{
		opts.printHelp(argv[0], out.getStream());
		return ExitCode::OK;
	}

	if(opts.version())
	{
		out.noPrefix();
		out() << Version::full() << cr;
		return ExitCode::OK;
	}

	if(opts.quiet())	out.quiet();
	if(opts.noPrefix())	out.noPrefix();
	if(opts.noColor())	out.colors = false;

	const auto& command = opts.getCommand();

	if(command.empty())
	{
		out(Ch::Error) << "No command provided." << cr;
		return ExitCode::NoCommand;
	}

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
		return ExitCode::UnknownCommand;
	}

	return cmdMap.at(command)(opts);
}
