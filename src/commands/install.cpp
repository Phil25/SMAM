#include "common.h"

#include "helpers/installer.h"

#include "../utils/smfs.h"

auto Command::install(const Opts& opts) noexcept -> ExitCode
{
    namespace fs = std::filesystem;

    const auto& addons = opts.getAddons();
    if (addons.empty())
    {
        out(Ch::Error) << "No addons specified." << cr;
        return ExitCode::NoAddons;
    }

    auto root = SMFS::findRoot(opts.getDestination().value_or(""));
    if (root)
    {
        fs::current_path(root.value());
    }
    else
    {
        out(Ch::Error) << "SourceMod root not found." << cr;
        return ExitCode::NoSMRoot;
    }

    if (!SMFS::loadData())
    {
        out(Ch::Error) << "No read/write permissions." << cr;
        return ExitCode::NoPermissions;
    }

    Installer installer(opts.getDbUrl(), addons, opts.force());
    auto      report = installer.installAll();

    if (!SMFS::writeData())
    {
        out(Ch::Error) << "Cannot write local addon metadata." << cr;
        return ExitCode::WriteError;
    }

    out(Ch::Info) << "Installation complete" << cr;

    using Type = Report::Type;
    report.print(Type::Installed);
    report.print(Type::Skipped);
    report.print(Type::Failed);

    return ExitCode::OK;
}
