#include "scraper.h"

#include <cassert>

Scraper::Vector Scraper::scrapers;
constexpr int   COUNT = 3;

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

void Scraper::add(ScraperPtr scraper) noexcept
{
    assert(scrapers.size() <= COUNT);
    scrapers.emplace_back(std::move(scraper));
}

auto Scraper::getData(const std::string& url) noexcept -> Data
{
    assert(scrapers.size() > 0 && "Scrapers not initialized");

    auto data = Data{};

    if (!url.empty())
    {
        for (const auto& scraper : scrapers)
        {
            if (scraper->match(url))
            {
                data = scraper->fetch(url);
            }
        }

        data.url = url;
    }

    return data;
}
