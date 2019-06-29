#include "scraper.h"

#include <cassert>

std::array<std::shared_ptr<Scraper>, Scraper::COUNT> Scraper::scrapers;

Scraper::Scraper(std::string_view url, const std::string& from,
                 const std::string& to) noexcept
    : aptUrl(url), dataFrom(from), dataTo(to)
{
}

Scraper::~Scraper() noexcept = default;

bool Scraper::match(const std::string& url) const noexcept
{
    return url.compare(0, aptUrl.size(), aptUrl) == 0;
}

void Scraper::make(int i, std::shared_ptr<Scraper> scraper) noexcept
{
    assert(i <= COUNT && "Invalid position");
    scrapers[i] = scraper;
}

auto Scraper::get(const std::string& url) noexcept -> MaybeScraper
{
    for (const auto& scraper : scrapers)
    {
        if (scraper->match(url))
        {
            return scraper;
        }
    }

    return std::nullopt;
}
