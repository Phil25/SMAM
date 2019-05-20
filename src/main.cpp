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
	bool force = opts.force();

	// prepare directories and add file to the addon list
	auto prep = [force](const fs::path& file, const std::string& id)
	{
		if(!SMFS::prepare(file.parent_path()))
		{
			out(Ch::Warn) << "Ignoring " << file << cr;
			return false;
		}

		bool exists = fs::exists(file);
		out(exists && !force ? Ch::Warn : Ch::Std)
			<< (exists ? "Overwriting " : "") << file << cr;

		SMFS::addFile(id, file);
		return true;
	};

	for(const auto& addon : addons)
	{
		if(SMFS::isInstalled(addon) && !force)
		{
			out(Ch::Info) << addon << " already installed." << cr;
			continue;
		}

		out(Ch::Info)
			<< Col::green
			<< "Installing " << addon << "..."
			<< Col::reset << cr;

		for(const auto& f : Installer::getFiles(addon, db))
		{
			fs::path file = f.path;
			file.append(f.name);
			if(!prep(file, addon)) continue;

			if(auto err = down.file(f.url, file); !err.empty())
			{
				out(Ch::Error) << err << cr;
				continue;
			}

			if(Archive::valid(file))
			{
				out(Ch::Info)
					<< "Extracting " << file.filename() << "..." << cr;

				Archive::extract(file, [prep, &addon](const fs::path& f)
				{
					return prep(f, addon);
				});
			}
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

		for(const auto& f : SMFS::getFiles(addon))
		{
			if(!fs::exists(f))
			{
				out() << "Skipping nonexistent file: " << f << cr;
				continue;
			}

			if(SMFS::countSharedFiles(f) > 1)
			{
				out() << "Skipping shared file: " << f << cr;
				continue;
			}

			out() << f << cr;
			fs::remove(f);
			SMFS::removeEmptyDirs(f);
		}

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
