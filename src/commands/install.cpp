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

    auto dest = opts.getDestination().value_or("");
    auto root = SMFS::Path::findRoot(dest);

    if (root)
    {
        fs::current_path(root.value());
    }
    else
    {
        out(Ch::Error) << "SourceMod root not found." << cr;
        return ExitCode::NoSMRoot;
    }

    if (!SMFS::Data::load())
    {
        out(Ch::Error) << "No read/write permissions." << cr;
        return ExitCode::NoPermissions;
    }

    Installer installer(opts.getDbUrl(), addons, opts.force());
    auto      report = installer.installAll();

    if (!SMFS::Data::save())
    {
        out(Ch::Error) << "Cannot write local addon metadata." << cr;
        return ExitCode::WriteError;
    }

    out(Ch::Info) << "Installation complete" << cr;

    report.print();

    return ExitCode::OK;
}
