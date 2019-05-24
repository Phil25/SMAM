#pragma once

#include <map>
#include <memory>
#include <vector>

#include "../downloader.h"

using Attachments = std::map<std::string, std::string>;

class Scraper
{
protected:
    Downloader&       downloader;
    const std::string aptUrl, dataFrom, dataTo;

    Scraper(Downloader& downloader, std::string_view url,
            const std::string& from = "",
            const std::string& to   = "") noexcept;

    virtual ~Scraper() noexcept;

public:
    // fetch file download urls
    virtual auto fetch(const std::string& url) noexcept
        -> Attachments = 0;

    // returns whether this scraper is applicable for given url
    bool match(const std::string& url) const noexcept;
};
