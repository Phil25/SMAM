#pragma once

#include <scrapers/scraper.h>

namespace smam
{
class LTScraper final : public Scraper
{
public:
    LTScraper() noexcept;
    virtual ~LTScraper() noexcept;

    auto Parse(const std::string& url) noexcept -> Data override;
};
}  // namespace smam
