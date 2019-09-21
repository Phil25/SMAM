#pragma once

#include <data/addon.h>
#include <net/database.h>
#include <scrapers/scraper.h>
#include <operations/core.hpp>

namespace smam
{
struct InstallerContext final
{
    std::array<ScraperPtr, 3> scrapers;
    Database                  database;

    std::set<std::string> pendingToBeInstalled;

    InstallerContext(const std::string&              databaseUrl,
                     const std::vector<std::string>& ids) noexcept;
};

class InitializeScrapers final : public Operation<InstallerContext>
{
public:
    InitializeScrapers(Logger&, InstallerContext&) noexcept;

    void Run() noexcept override;
};

class CheckPending final : public Operation<InstallerContext>
{
    std::string currentID;

public:
    CheckPending(Logger&, InstallerContext&,
                 std::string currentID) noexcept;

    void Run() noexcept override;
};
}  // namespace smam
