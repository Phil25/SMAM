#include <commands/common.h>  // ExitCode
#include <utils/options.h>
#include <version.hpp>

#include <unistd.h>  // getuid

int main(int argc, const char* argv[])
{
    using namespace smam;

    auto       logger  = Logger();
    const auto options = Options(argc, argv, logger);

    if (options.Help())
    {
        logger << options.GenHelp(argv[0]);
        return ExitCode::OK;
    }

    if (options.Version())
    {
        logger << version::Full() << cr;
        return ExitCode::OK;
    }

    if (!getuid() && !options.AllowRoot())
    {
        logger.Error() << "SMAM should not be run as root." << cr;
        return ExitCode::RunAsRoot;
    }

    logger.SetPrefix(!options.NoPrefix());
    logger.SetColor(!options.NoColor());
    logger.SetOutput(!options.Quiet());

    const auto& command = options.Command();

    if (command.empty())
    {
        logger.Error() << "No command provided." << cr;
        return ExitCode::NoCommand;
    }

    if (!command::Exists(command))
    {
        logger.Error() << "Unknown command: \"" << command << "\""
                       << cr;
        return ExitCode::UnknownCommand;
    }

    return command::Run(logger, command, options);
}
