#pragma once

#include "core.hpp"

#include <data/addon.h>
#include <scrapers/scraper.h>

namespace smam
{
struct AddonContext
{
    AddonPtr      addon;
    Scraper::Data data;
};

class InitAddonContext final : public Operation<AddonContext>
{
public:
    InitAddonContext(Logger&, AddonContext&, const AddonPtr&) noexcept;

    void Run() noexcept override;
};

class FindData final : public Operation<AddonContext>
{
    const ScraperArray& scrapers;
    const std::string&  url;

public:
    FindData(Logger&, AddonContext&,
             std::reference_wrapper<const ScraperArray>,
             const std::string& url) noexcept;

    void Run() noexcept override;
};

class EvaluateFiles final : public Operation<AddonContext>
{
public:
    EvaluateFiles(Logger&, AddonContext&) noexcept;

    void Run() noexcept override;
};

class DownloadFiles final : public Operation<AddonContext>
{
public:
    DownloadFiles(Logger&, AddonContext&) noexcept;

    void Run() noexcept override;
};

class MarkInstalled final : public Operation<AddonContext>
{
public:
    MarkInstalled(Logger&, AddonContext&) noexcept;

    void Run() noexcept override;
};
}  // namespace smam
