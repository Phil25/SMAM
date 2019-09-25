#pragma once

#include <data/addon.h>
#include <scrapers/scraper.h>
#include <operations/core.hpp>

namespace smam
{
struct InstallerContext final
{
    ScraperArray  scrapers;
    Scraper::Data parsed;

    std::map<std::string, AddonPlan> data;
    std::set<std::string>            pendingToBeInstalled;

    AddonPtr    addon;
    std::string url;
};

class PrecacheAddons final : public Operation<InstallerContext>
{
    const std::string&              databaseUrl;
    const std::vector<std::string>& ids;

public:
    PrecacheAddons(Logger&, InstallerContext&,
                   const std::string&              databaseUrl,
                   const std::vector<std::string>& ids) noexcept;

    void Run() noexcept override;
};

class InitScrapers final : public Operation<InstallerContext>
{
public:
    InitScrapers(Logger&, InstallerContext&) noexcept;

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

class SetAddon final : public Operation<InstallerContext>
{
    std::string currentID;

public:
    SetAddon(Logger&, InstallerContext&,
             std::string currentID) noexcept;

    void Run() noexcept override;
};

class CheckInstalled final : public Operation<InstallerContext>
{
    bool force;

public:
    CheckInstalled(Logger&, InstallerContext&, bool force) noexcept;

    void Run() noexcept override;
};

class InstallDependencies final : public Operation<InstallerContext>
{
public:
    InstallDependencies(Logger&, InstallerContext&) noexcept;

    void Run() noexcept override;
};

class InstallAddon final : public Operation<InstallerContext>
{
public:
    InstallAddon(Logger&, InstallerContext&) noexcept;

    void Run() noexcept override;
};
}  // namespace smam
