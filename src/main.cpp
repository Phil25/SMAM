#include "opts.hpp"
#include "version.hpp"

#include "utils/archive.h"
#include "utils/printer.h"
#include "utils/smfs.h"

#include "installer.h"
#include "downloader.h"

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

namespace Cmd
{
	using fptr = int (*)(const Opts&);
	int install	(const Opts&);
	int remove	(const Opts&);
	int info	(const Opts&);
	int search	(const Opts&);
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

	const std::map<std::string_view, Cmd::fptr> cmds{
		{"install",		Cmd::install},
		{"remove",		Cmd::remove},
		{"uninstall",	Cmd::remove},
		{"info",		Cmd::info},
		{"search",		Cmd::search},
	};

	if(!cmds.count(command))
	{
		out(Ch::Error) << "Unknown command: \"" << command << '\"' << cr;
		return ExitCode::UnknownCommand;
	}

	return cmds.at(command)(opts);
}

int Cmd::install(const Opts& opts)
{
	const auto& addons = opts.getAddons();
	if(addons.empty())
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
	db.precache(addons);

	for(const auto& addon : addons)
	{
		if(SMFS::isInstalled(addon) && !opts.force())
		{
			out(Ch::Info) << addon << " already installed." << cr;
			continue;
		}

		out(Ch::Info)
			<< Col::green
			<< "Installing " << addon << "..."
			<< Col::reset << cr;

		bool success = Installer::setup(addon, db, [&](const File& f)
		{
			fs::path file = f.path;
			file.append(f.name);
			if(!SMFS::regFile(file, addon)) return false;

			if(auto err = down.file(f.url, file); !err.empty())
			{
				out(Ch::Error) << err << cr;
				return false;
			}

			if(!Archive::valid(file)) return true; // no further action

			out(Ch::Info) << "Extracting " << f.name << "..." << cr;

			return Archive::extract(file, [&](const fs::path& extracted)
			{
				return SMFS::regFile(extracted, addon);
			});
		});

		if(!success)
		{
			out(Ch::Error) << addon << " failed to install." << cr;
			SMFS::removeAddon(addon, false);
		}
	}

	if(!SMFS::writeData())
	{
		out(Ch::Error) << "Cannot write local addon metadata." << cr;
		return ExitCode::WriteError;
	}

	return ExitCode::OK;
}

int Cmd::remove(const Opts& opts)
{
	auto addons = opts.getAddons();
	if(addons.empty())
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

	for(const auto& addon : addons)
	{
		if(!SMFS::isInstalled(addon))
		{
			out(Ch::Warn) << "Addon not installed: " << addon << cr;
			continue;
		}

		out(Ch::Info)
			<< Col::red
			<< "Removing " << addon << "..."
			<< Col::reset << cr;

		SMFS::removeAddon(addon);
	}

	if(!SMFS::writeData())
	{
		out(Ch::Error) << "Cannot write local addon metadata." << cr;
		return ExitCode::WriteError;
	}

	return ExitCode::OK;
}

int Cmd::info(const Opts&)
{
	return ExitCode::OK;
}

int Cmd::search(const Opts&)
{
	return ExitCode::OK;
}
