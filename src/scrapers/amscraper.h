#pragma once

#include "scraper.h"

class AMScraper final : public Scraper
{
public:
    AMScraper() noexcept;
    virtual ~AMScraper() noexcept;

    auto fetch(const std::string& url) noexcept -> Attachments override;
};
