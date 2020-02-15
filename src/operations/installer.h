#pragma once

#include <data/addon.h>
#include <scrapers/scraper.h>
#include <operations/core.hpp>

#include <filesystem>

namespace smam
{
struct InstallerContext final
{
    std::string id;
    AddonMapPtr cache;

    AddonPtr addon;

    std::set<std::string> pendingToBeInstalled;
    std::filesystem::path root;

    explicit InstallerContext(std::string        id,
                              const AddonMapPtr& cache) noexcept;
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
    ScraperArrayPtr scrapers;
    bool            noDeps;

public:
    InstallDependencies(const LoggerPtr&, InstallerContext&,
                        const ScraperArrayPtr&, bool noDeps) noexcept;

    void Run() noexcept override;
};

class InstallAddon final : public Operation<InstallerContext>
{
    ScraperArrayPtr scrapers;

public:
    InstallAddon(const LoggerPtr&, InstallerContext&,
                 const ScraperArrayPtr&) noexcept;

    void Run() noexcept override;
};

class BeginTransaction final : public Operation<InstallerContext>
{
public:
    BeginTransaction(const LoggerPtr&, InstallerContext&) noexcept;

    void Run() noexcept override;
};

class CommitTransaction final : public Operation<InstallerContext>
{
public:
    CommitTransaction(const LoggerPtr&, InstallerContext&) noexcept;

    void Run() noexcept override;
};
}  // namespace smam
