#include "common.h"

#include <commands/helpers/installer.h>

auto Command::install(const Opts& opts) noexcept -> ExitCode
{
    const auto& addons = opts.getAddons();

    if (Common::noAddons(addons)) return ExitCode::NoAddons;
    if (Common::noSMRoot(opts)) return ExitCode::NoSMRoot;
    if (auto ret = Common::load(); ret) return ret;

    Installer installer(opts.getDbUrl(), addons, opts.force(),
                        opts.noDeps());

    const auto& report = installer.installAll();

    if (!Common::save()) return ExitCode::WriteError;

    out(Ch::Info) << "Installation complete" << cr;

    report.print();

    return ExitCode::OK;
}
