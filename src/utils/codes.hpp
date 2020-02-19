#pragma once

namespace smam
{
enum ExitCode
{
    OK        = 0,
    RunAsRoot = 1,
    NoPermissions,
    WriteError,
    NoCommand,
    UnknownCommand,
    NoAddons,
    NoSMRoot,
    BadCache,
    DatabaseFailure,
};
}
