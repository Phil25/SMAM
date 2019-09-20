#pragma once

#include <scrapers/scraper.h>

namespace smam
{
class GHScraper final : public Scraper
{
public:
    GHScraper() noexcept;
    virtual ~GHScraper() noexcept;

    auto Parse(const std::string& url) noexcept -> Data override;
};
}  // namespace smam
