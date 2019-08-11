#include "common.h"

#include <scrapers/amscraper.h>
#include <scrapers/ghscraper.h>
#include <scrapers/ltscraper.h>

#include <database.h>
#include <utils/report.h>

/*
 * Helper class for setting up the installation environment and
 * providing methods and order to complete it. Logic to take care of
 * dependency resolution is here.
 */
class Installer final
{
    using Type         = Report::Type;
    using StringVector = std::vector<std::string>;

    Database database;

    const StringVector& ids;

    bool forceInstall;
    bool noDeps;

    Report                report;
    std::set<std::string> pending;

public:
    Installer(const std::string& databaseUrl, const StringVector& ids,
              bool forceInstall, bool noDeps = false) noexcept;

    /*
     * Install every addon in the `addons` vector.
     */
    auto installAll() noexcept -> Report;

private:
    /*
     * Install a single specific addon that has been precached by the
     * database.
     */
    auto installSingle(const std::string& addon) noexcept
        -> Report::Type;

    /*
     * Returns the processed data of a specified addon.
     */
    auto get(const std::string& url) noexcept -> Scraper::Data;
};

auto Command::install(const Opts& opts) noexcept -> ExitCode
{
    const auto& addons = opts.getAddons();

    if (Common::noAddons(addons)) return ExitCode::NoAddons;
    if (Common::noSMRoot(opts)) return ExitCode::NoSMRoot;
    if (auto ret = Common::load(); ret) return ret;

    auto installer =
        Installer(opts.getDbUrl(), addons, opts.force(), opts.noDeps());

    const auto& report = installer.installAll();

    if (!Common::save()) return ExitCode::WriteError;

    out(Ch::Info) << "Installation complete." << cr;

    report.print();

    return ExitCode::OK;
}

Installer::Installer(const std::string&  databaseUrl,
                     const StringVector& ids, bool forceInstall,
                     bool noDeps) noexcept
    : database(databaseUrl),
      ids(ids),
      forceInstall(forceInstall),
      noDeps(noDeps)
{
    Scraper::make(0, std::make_shared<AMScraper>());
    Scraper::make(1, std::make_shared<LTScraper>());
    Scraper::make(2, std::make_shared<GHScraper>());

    if (!database.precache(ids))
    {
        out(Ch::Error) << "Parsing remote failed." << cr;
    }
}

auto Installer::installAll() noexcept -> Report
{
    for (const auto& id : ids)
    {
        auto result = installSingle(id);
        report.insert(result, id);

        if (result == Type::Installed)
        {
            Addon::get(id).value()->markExplicit();
        }

        out << cr;
    }

    return report;
}

/*
 * Install a single specific addon that has been precached by the
 * database.
 */
auto Installer::installSingle(const std::string& id) noexcept -> Type
{
    if (pending.count(id))  // already being installed
    {
        return Type::Queued;
    }

    if (Addon::isInstalled(id) && !forceInstall)
    {
        out(Ch::Info) << "Already installed: " << id << cr;
        return Type::Skipped;
    }

    auto planVar = database.get(id);

    if (std::holds_alternative<std::string>(planVar))  // holds error
    {
        out(Ch::Error) << Col::red << '(' << id << ") "
                       << std::get<std::string>(planVar) << Col::reset
                       << cr;
        return Type::Failed;
    }

    pending.insert(id);  // circumvent cyclic deps inf loop

    auto plan  = std::get<Database::Plan>(planVar);
    auto data  = get(plan.first);
    auto addon = plan.second;

    bool success = addon->forEachDep([&](const auto& dep) {
        if (noDeps)  // don't automatically install dependencies
        {
            if (!Addon::isInstalled(dep))
            {
                report.remark(id, dep, Type::Ignored);
            }
        }
        else
        {
            auto depResult = installSingle(dep);
            report.remark(id, dep, depResult);

            if (depResult == Type::Failed) return false;
        }

        out << cr;
        return true;
    });

    out(Ch::Info) << Col::green << "Installing " << id << "..."
                  << Col::reset << cr;

    success &= addon->install(data);

    if (!success)
    {
        out(Ch::Error) << Col::red << "Failed to install " << id
                       << Col::reset << cr;

        addon->remove();
        return Type::Failed;
    }

    return Type::Installed;
}

auto Installer::get(const std::string& url) noexcept -> Scraper::Data
{
    Scraper::Data data;

    if (!url.empty())
    {
        if (auto scraper = Scraper::get(url))
        {
            data = scraper->get()->fetch(url);
        }

        data.url = url;
    }

    return data;
}
