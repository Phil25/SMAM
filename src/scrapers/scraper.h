#pragma once

#include <map>
#include <memory>
#include <vector>

namespace smam
{
class Scraper
{
public:
    struct Data final
    {
        enum class Website
        {
            AlliedModders,
            Limetech,
            GitHub,
            Unknown
        };

        std::map<std::string, std::string> nameToLink;
        std::string                        url{};
        Website                            website{Website::Unknown};
    };

protected:
    const std::string url, from, to;

    Scraper(std::string_view url, std::string from = "",
            std::string to = "") noexcept;

public:
    virtual ~Scraper() noexcept;

    auto Url() const noexcept -> const std::string&;

    virtual auto Parse(const std::string& url) noexcept -> Data = 0;
};

using ScraperPtr      = std::unique_ptr<Scraper>;
using ScraperArray    = std::array<ScraperPtr, 3>;
using ScraperArrayPtr = std::shared_ptr<ScraperArray>;
}  // namespace smam
