#include "scraper.h"

Scraper::Scraper(Downloader& downloader, strv url, cstr& from, cstr& to):
	downloader(downloader),
	aptUrl(url),
	dataFrom(from),
	dataTo(to)
{
}

Scraper::~Scraper()
{
}

/*
 * Is this Scraper appropriate for given URL.
 * Used in `installer` when searching for a valid Scraper.
 */
bool Scraper::match(const std::string& url) const
{
	return url.compare(0, aptUrl.size(), aptUrl) == 0;
}
