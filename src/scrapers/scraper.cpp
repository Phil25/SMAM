#include "scraper.h"

namespace smam
{
Scraper::Scraper(std::string_view url, std::string from,
                 std::string to) noexcept
    : url(url), from(std::move(from)), to(std::move(to))
{
}

Scraper::~Scraper() noexcept = default;

auto Scraper::Url() const noexcept -> const std::string&
{
    return url;
}
}  // namespace smam
