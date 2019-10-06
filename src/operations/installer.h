#pragma once

#include <data/addon.h>
#include <scrapers/scraper.h>
#include <operations/core.hpp>

namespace smam
{
struct InstallerContext final
{
    std::string id;
    AddonMap    cache;

    AddonPtr addon;

    std::set<std::string> pendingToBeInstalled;

    explicit InstallerContext(std::string id, AddonMap cache) noexcept;
};

class CheckPending final : public Operation<InstallerContext>
{
public:
    CheckPending(const LoggerPtr&, InstallerContext&) noexcept;

    void Run() noexcept override;
};

class ParseCache final : public Operation<InstallerContext>
{
public:
    ParseCache(const LoggerPtr&, InstallerContext&) noexcept;

    void Run() noexcept override;
};

class MarkExplicit final : public Operation<InstallerContext>
{
public:
    MarkExplicit(const LoggerPtr&, InstallerContext&) noexcept;

    void Run() noexcept override;
};

class CheckInstalled final : public Operation<InstallerContext>
{
    bool force;

public:
    CheckInstalled(const LoggerPtr&, InstallerContext&,
                   bool force) noexcept;

    void Run() noexcept override;
};

class InstallDependencies final : public Operation<InstallerContext>
{
    std::shared_ptr<ScraperArray> scrapers;

public:
    InstallDependencies(const LoggerPtr&, InstallerContext&,
                        const std::shared_ptr<ScraperArray>&) noexcept;

    void Run() noexcept override;
};

class InstallAddon final : public Operation<InstallerContext>
{
    std::shared_ptr<ScraperArray> scrapers;

public:
    InstallAddon(const LoggerPtr&, InstallerContext&,
                 const std::shared_ptr<ScraperArray>&) noexcept;

    void Run() noexcept override;
};
}  // namespace smam
