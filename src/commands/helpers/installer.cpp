#include "installer.h"

#include <filesystem>
#include <functional>
#include <regex>

#include <download.h>

#include <smfs/addon.h>
#include <smfs/path.h>

#include <scrapers/amscraper.h>
#include <scrapers/ghscraper.h>
#include <scrapers/ltscraper.h>
#include <scrapers/scraper.h>

#include <utils/archive.h>
#include <utils/misc.h>
#include <utils/printer.h>
#include <utils/version.h>

namespace fs       = std::filesystem;
using StringVector = std::vector<std::string>;
using Type         = Report::Type;

Installer::Installer(const std::string&  databaseUrl,
                     const StringVector& ids, bool forceInstall,
                     bool noDeps) noexcept
    : database(databaseUrl),
      ids(ids),
      forceInstall(forceInstall),
      noDeps(noDeps),
      failedCount(0)
{
    Scraper::make(0, std::make_shared<AMScraper>());
    Scraper::make(1, std::make_shared<LTScraper>());
    Scraper::make(2, std::make_shared<GHScraper>());

    database.precache(ids);
}

auto Installer::installAll() noexcept -> const Report&
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

    auto planOpt = database.get(id);

    if (!planOpt.has_value())
    {
        out(Ch::Error) << Col::red << "Not found: " << id << Col::reset
                       << cr;
        return Type::Failed;
    }

    pending.insert(id);  // circumvent cyclic deps inf loop
    auto data    = get(planOpt.value().first);
    auto addon   = planOpt.value().second;
    bool success = true;

    // TODO: move to Addon, delete Addon::getDeps
    for (const auto& dep : addon->getDeps())
    {
        if (noDeps)
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

            if (depResult == Type::Failed)
            {
                success = false;
                break;
            }
        }
    }

    out(Ch::Info) << Col::green << "Installing " << id << "..."
                  << Col::reset << cr;

    success &= addon->install(data);

    if (!success)
    {
        out(Ch::Error) << Col::red << "Failed to install " << addon
                       << Col::reset << cr << cr;

        addon->remove();
        return Type::Failed;
    }

    out << cr;
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
