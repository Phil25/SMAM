#pragma once

#include <map>
#include <memory>
#include <vector>

class Scraper
{
public:
    using MaybeScraper = std::optional<std::shared_ptr<Scraper>>;

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

private:
    constexpr static int COUNT = 3;

    static std::array<std::shared_ptr<Scraper>, COUNT> scrapers;

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
     * Add an instance of a scraper to the global list
     */
    static void make(int i, std::shared_ptr<Scraper> scraper) noexcept;

    /*
     * Return appropriate Scraper for given URL or null.
     */
    static auto get(const std::string& url) noexcept -> MaybeScraper;
};
