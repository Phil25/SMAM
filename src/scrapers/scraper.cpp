#include "scraper.h"

Scraper::Scraper(Downloader& downloader, std::string_view url,
                 const std::string& from,
                 const std::string& to) noexcept
    : downloader(downloader), aptUrl(url), dataFrom(from), dataTo(to)
{
}

Scraper::~Scraper() noexcept = default;

/*
 * Is this Scraper appropriate for given URL.
 * Used in `installer` when searching for a valid Scraper.
 */
bool Scraper::match(const std::string& url) const noexcept
{
    return url.compare(0, aptUrl.size(), aptUrl) == 0;
}
