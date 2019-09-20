#include "ghscraper.h"

#include <net/download.h>

#include <nlohmann/json.hpp>

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
inline auto GetRepoUrl(const std::string& url) -> std::string
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
inline auto GetReleasesUrl(const std::string& repoUrl) -> std::string
{
    return std::string(URL_API).append(repoUrl).append(RELEASES);
}
}  // namespace

namespace smam
{
GHScraper::GHScraper() noexcept : Scraper(URL)
{
}

GHScraper::~GHScraper() noexcept = default;

auto GHScraper::Parse(const std::string& url) noexcept -> Data
{
    auto release = GetReleasesUrl(GetRepoUrl(url));
    auto doc     = download::Html(release);

    Data data;
    data.website = Data::Website::GitHub;

    try
    {
        json root = json::parse(doc);

        std::string name, url;

        for (const auto& asset : root.at("assets"))
        {
            data[asset.at("name")] = asset.at("browser_download_url");
        }
    }
    catch (const json::exception&)
    {
    }

    return data;
}
}  // namespace smam
