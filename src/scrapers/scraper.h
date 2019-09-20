#pragma once

#include <map>
#include <memory>
#include <vector>

namespace smam
{
class Scraper
{
public:
    struct Data : std::map<std::string, std::string>
    {
        enum class Website
        {
            AlliedModders,
            Limetech,
            GitHub,
            Unknown
        };

        std::string url{};
        Website     website{Website::Unknown};
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

using ScraperPtr = std::unique_ptr<Scraper>;
}  // namespace smam
