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
		{"list",		Cmd::info},
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
	int failed = 0;

	for(const auto& addon : addons)
	{
		if(SMFS::isInstalled(addon) && !opts.force())
		{
			out(Ch::Info) << "Already installed: " << addon << cr;
			continue;
		}

		if(!db.isPrecached(addon))
		{
			out(Ch::Error)
				<< Col::red << "Not found: "
				<< addon << Col::reset << cr;

			++failed;
			continue;
		}

		out(Ch::Info)
			<< Col::green
			<< "Installing " << addon << "..."
			<< Col::reset << cr;

		auto regFile = [&](const fs::path& file)
		{
			if(!SMFS::prepare(file.parent_path()))
			{
				out(Ch::Warn) << "Ignoring " << file << cr;
				return false;
			}

			bool exists = SMFS::fs::exists(file);
			out(exists ? Ch::Warn : Ch::Std)
				<< (exists ? "Overwriting " : "") << file << cr;

			SMFS::addFile(file, addon);
			return true;
		};

		bool success = Installer::setup(addon, db, [&](const File& f)
		{
			fs::path file = f.path;
			file.append(f.name);
			if(!regFile(file)) return false;

			if(auto err = down.file(f.url, file); !err.empty())
			{
				out(Ch::Error) << err << cr;
				return false;
			}

			if(!Archive::valid(file)) return true; // no further action

			out(Ch::Info) << "Extracting " << f.name << "..." << cr;

			bool res = Archive::extract(file, [&](const fs::path& f)
			{
				return regFile(f);
			});

			return res && SMFS::eraseFile(file, addon);
		});

		if(!success)
		{
			out(Ch::Error)
				<< Col::red << "Failed to install " << addon
				<< Col::reset << cr;

			for(const auto& file : SMFS::getFiles(addon))
			{
				SMFS::removeFile(file);
			}

			SMFS::removeAddon(addon);
			++failed;
		}
	}

	if(!SMFS::writeData())
	{
		out(Ch::Error) << "Cannot write local addon metadata." << cr;
		return ExitCode::WriteError;
	}

	if(failed) out(Ch::Error)
		<< Col::red << failed << " addon(s) failed to install."
		<< Col::reset << cr;

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
			<< Col::yellow
			<< "Removing " << addon << "..."
			<< Col::reset << cr;

		for(const auto& file : SMFS::getFiles(addon))
		{
			switch(SMFS::removeFile(file))
			{
			case SMFS::DeleteResult::NotExists:
				out() << "Skipping non-existent file: " << file << cr;
				break;

			case SMFS::DeleteResult::Shared:
				out() << "Skipping shared file: " << file << cr;
				break;

			case SMFS::DeleteResult::OK:
				out() << file << cr;
				break;
			}
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

int Cmd::info(const Opts& opts)
{
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

	const auto& filter = opts.getAddons();

	if(filter.empty())
	{
		out(Ch::Info) << "Installed addons:" << cr;

		SMFS::getInstalled([](const std::string& addon)
		{
			out() << addon << " ("
				<< SMFS::getFiles(addon).size() << " file(s))" << cr;
		});

		return ExitCode::OK;
	}

	for(const auto& addon : filter)
	{
		if(!SMFS::isInstalled(addon))
		{
			out(Ch::Warn) << "Not installed: " << addon << cr;
			continue;
		}

		const auto& files = SMFS::getFiles(addon);

		out(Ch::Info) << Col::green << addon << Col::reset
			<< " (" << files.size() << ')' << cr;

		for(const auto& file : files) out() << file << cr;
	}

	return ExitCode::OK;
}

int Cmd::search(const Opts&)
{
	return ExitCode::OK;
}
