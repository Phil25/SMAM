#pragma once

#include <string>
#include <vector>

#include "report.h"

#include "../../database.h"

/*
 * Helper class for setting up the installation environment and
 * providing the helper methods to complete it.
 */
class Installer final
{
    using StringVector = std::vector<std::string>;

    Database database;

    const StringVector& addons;

    bool forceInstall;
    int  failedCount;

public:
    Installer(const std::string&  databaseUrl,
              const StringVector& addons, bool forceInstall) noexcept;

    /*
     * Install every addon in the `addons` vector.
     */
    auto installAll() noexcept -> Report;

    /*
     * Install a single specific addon that has been precached by the
     * database.
     */
    auto installSingle(const std::string& addon) noexcept
        -> Report::Type;

    /*
     * Returns the processed files of a specified addon.
     */
    auto getAddonFiles(const std::string& id) noexcept
        -> std::vector<File>;
};
