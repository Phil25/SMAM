#pragma once

#include <set>
#include <string>
#include <vector>

#include <commands/helpers/report.h>
#include <database.h>

/*
 * Helper class for setting up the installation environment and
 * providing the helper methods to complete it.
 */
class Installer final
{
    using StringVector = std::vector<std::string>;

    Database database;

    const StringVector& ids;

    bool forceInstall;
    bool noDeps;
    int  failedCount;

    Report                report;
    std::set<std::string> pending;

public:
    Installer(const std::string& databaseUrl, const StringVector& ids,
              bool forceInstall, bool noDeps = false) noexcept;

    /*
     * Install every addon in the `addons` vector.
     */
    auto installAll() noexcept -> const Report&;

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
