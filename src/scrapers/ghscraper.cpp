#include "ghscraper.h"

#include <nlohmann/json.hpp>

#include "../download.h"

#include "../utils/printer.h"

using json = nlohmann::json;

namespace
{
constexpr std::string_view URL      = "https://github.com/";
constexpr std::string_view URL_API  = "https://api.github.com/repos/";
constexpr std::string_view RELEASES = "releases/latest";

/*
 * Get GitHub repository address from a full link.
 * https://github.com/user/repo/ -> user/repo/
 */
inline auto getRepoUrl(const std::string& url) -> std::string
{
    std::string repoUrl = url.substr(URL.size());

    if (repoUrl.at(repoUrl.size() - 1) != '/')
    {
        repoUrl += '/';
    }

    return repoUrl;
}

/*
 * Get API URL to the latest releases from a particular GitHub
 * repository address.
 * user/repo -> https://api.github.com/repos/user/repo/releases/latest
 */
inline auto getReleasesUrl(const std::string& repoUrl) -> std::string
{
    return std::string(URL_API).append(repoUrl).append(RELEASES);
}
}  // namespace

GHScraper::GHScraper() noexcept : Scraper(URL) {}

GHScraper::~GHScraper() noexcept = default;

auto GHScraper::fetch(const std::string& url) noexcept -> Data
{
    auto release = getReleasesUrl(getRepoUrl(url));
    auto page    = Download::page(release);

    Data data;
    data.website = Data::Website::GitHub;

    try
    {
        json root = json::parse(page);

        std::string name, url;

        for (const auto& asset : root.at("assets"))
        {
            data[asset.at("name")] = asset.at("browser_download_url");
        }
    }
    catch (const json::exception& e)
    {
        out(Ch::Error) << e.what() << cr;
    }

    return data;
}
