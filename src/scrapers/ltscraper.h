#pragma once

#include "scraper.h"

class LTScraper : public Scraper
{
public:
    LTScraper(Downloader&) noexcept;
    virtual ~LTScraper() noexcept;

    auto fetch(const std::string& url) noexcept -> Attachments override;
};
