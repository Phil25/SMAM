#pragma once

#include "scraper.h"

class GHScraper : public Scraper
{
public:
    GHScraper(Downloader&) noexcept;
    virtual ~GHScraper() noexcept;

    auto fetch(const std::string& url) noexcept -> Attachments override;
};
