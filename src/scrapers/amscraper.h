#pragma once

#include "scraper.h"

class AMScraper : public Scraper
{
public:
    AMScraper(Downloader&) noexcept;
    virtual ~AMScraper() noexcept;

    auto fetch(const std::string& url) noexcept -> Attachments override;
};
