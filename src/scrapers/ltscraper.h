#pragma once

#include <scrapers/scraper.h>

class LTScraper final : public Scraper
{
public:
    LTScraper() noexcept;
    virtual ~LTScraper() noexcept;

    auto fetch(const std::string& url) noexcept -> Data override;
};
