#pragma once

#include "core.hpp"

#include <data/addon.h>
#include <scrapers/scraper.h>

namespace smam
{
struct AddonContext final
{
    AddonPtr      addon;
    Scraper::Data data;

    explicit AddonContext(const AddonPtr&) noexcept;
};

class FindData final : public Operation<AddonContext>
{
    ScraperArrayPtr scrapers;

public:
    FindData(const LoggerPtr&, AddonContext&,
             const ScraperArrayPtr&) noexcept;

    void Run() noexcept override;
};

class EvaluateFiles final : public Operation<AddonContext>
{
public:
    EvaluateFiles(const LoggerPtr&, AddonContext&) noexcept;

    void Run() noexcept override;
};

class DownloadFiles final : public Operation<AddonContext>
{
public:
    DownloadFiles(const LoggerPtr&, AddonContext&) noexcept;

    void Run() noexcept override;
};

class MarkInstalled final : public Operation<AddonContext>
{
public:
    MarkInstalled(const LoggerPtr&, AddonContext&) noexcept;

    void Run() noexcept override;
};

class ExtractArchives final : public Operation<AddonContext>
{
public:
    ExtractArchives(const LoggerPtr&, AddonContext&) noexcept;

    void Run() noexcept override;
};

class CheckSatisfied final : public Operation<AddonContext>
{
public:
    CheckSatisfied(const LoggerPtr&, AddonContext&) noexcept;

    void Run() noexcept override;
};
}  // namespace smam
