#pragma once

#include <map>
#include <memory>
#include <vector>

class Scraper
{
    using ScraperPtr = std::unique_ptr<Scraper>;
    using Vector     = std::vector<ScraperPtr>;

    static Vector scrapers;

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

        std::string url     = {};
        Website     website = Website::Unknown;
    };

protected:
    const std::string aptUrl, dataFrom, dataTo;

    Scraper(std::string_view url, const std::string& from = "",
            const std::string& to = "") noexcept;

public:
    virtual ~Scraper() noexcept;

    /*
     * Fetch file download urls.
     */
    virtual auto fetch(const std::string& url) noexcept -> Data = 0;

    /*
     * Return whether this scraper is applicable for given url.
     */
    bool match(const std::string& url) const noexcept;

    /*
     * Add an instance of a scraper to the static list
     */
    static void add(ScraperPtr scraper) noexcept;

    /*
     * Return appropriate Scraper for given URL or null.
     */
    static auto getData(const std::string& url) noexcept -> Data;
};
