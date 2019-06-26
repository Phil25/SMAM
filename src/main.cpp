#include "version.hpp"

#include "utils/printer.h"

#include "commands/common.h"  // ExitCode

#include <unistd.h>  // getuid
#include <filesystem>

namespace fs = std::filesystem;

int main(int argc, const char* argv[])
{
    const Opts opts(argc, argv);

    if (opts.help())
    {
        opts.printHelp(argv[0], out);
        return ExitCode::OK;
    }

    if (opts.version())
    {
        out.setPrefix(false);
        out << Version::full() << cr;
        return ExitCode::OK;
    }

    if (!getuid() && !opts.allowRoot())
    {
        out(Ch::Error) << "SMAM should not be ran as root." << cr;
        return ExitCode::RanAsRoot;
    }

    out.setPrefix(!opts.noPrefix());
    out.setColor(!opts.noColor());
    out.setOutput(!opts.quiet());

    const auto& command = opts.getCommand();

    if (command.empty())
    {
        out(Ch::Error) << "No command provided." << cr;
        return ExitCode::NoCommand;
    }

    if (!Command::exists(command))
    {
        out(Ch::Error) << "Unknown command: \"" << command << '\"'
                       << cr;
        return ExitCode::UnknownCommand;
    }

    return Command::run(command, opts);
}
