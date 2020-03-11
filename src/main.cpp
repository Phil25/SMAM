#include <commands/common.h>  // ExitCode
#include <utils/options.h>
#include <version.hpp>

#include <unistd.h>  // getuid

int main(int argc, const char* argv[])
{
    using namespace smam;

    const auto logger  = std::make_shared<Logger>();
    const auto options = std::make_shared<Options>(argc, argv, logger);

    if (options->Help() || options->InFailedState())
    {
        logger->Out() << options->GenHelp(argv[0]);
        return ExitCode::OK;
    }

    if (options->Version())
    {
        logger->Out() << version::Full() << cr;
        return ExitCode::OK;
    }

    logger->SetPrefix(!options->NoPrefix());
    logger->SetColor(!options->NoColor());
    logger->SetOutput(!options->Quiet());

    if (!getuid() && !options->AllowRoot())
    {
        logger->Error() << "SMAM should not be run as root." << cr;
        return ExitCode::RunAsRoot;
    }

    const auto& command = options->Command();

    if (command.empty())
    {
        logger->Error() << "No command provided." << cr;
        return ExitCode::NoCommand;
    }

    if (!command::Exists(command))
    {
        logger->Error() << "Unknown command: \"" << command << "\""
                        << cr;
        return ExitCode::UnknownCommand;
    }

    return command::Run(logger, command, options);
}
