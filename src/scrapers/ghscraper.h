#pragma once

#include "scraper.h"

class GHScraper final : public Scraper
{
public:
    GHScraper() noexcept;
    virtual ~GHScraper() noexcept;

    auto fetch(const std::string& url) noexcept -> Data override;
};
