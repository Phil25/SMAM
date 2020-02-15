#pragma once

#include <scrapers/scraper.h>

namespace smam
{
class AMScraper final : public Scraper
{
public:
    AMScraper() noexcept;
    virtual ~AMScraper() noexcept;

    auto Parse(const std::string& url) noexcept -> Data override;
};
}  // namespace smam
