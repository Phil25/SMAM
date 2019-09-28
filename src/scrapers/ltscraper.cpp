#include "ltscraper.h"

#include <net/download.h>
#include <utils/common.h>

#include <pugixml.hpp>

namespace
{
constexpr std::string_view URL = "https://builds.limetech.io/";

auto ParseData(const std::string& doc) noexcept
{
    auto root = pugi::xml_document{};
    auto data = smam::Scraper::Data{};
    auto name = std::string{};
    auto url  = std::string{};

    root.load_string(doc.c_str());

    for (const auto& release : root.select_nodes("//td[position()=2]"))
    {
        auto anchor = release.node().first_child();
        if (std::string{anchor.name()} != "a") continue;

        url = anchor.attribute("href").as_string();
        url.insert(0, URL);

        break;  // need only the first one
    }

    auto pos = url.rfind('/');
    if (pos != std::string::npos)
    {
        name = url.substr(++pos);
    }

    data.nameToLink.emplace(name, url);
    return data;
}
}  // namespace

namespace smam
{
LTScraper::LTScraper() noexcept : Scraper(URL, "<tbody>", "</tbody>")
{
}

LTScraper::~LTScraper() noexcept = default;

auto LTScraper::Parse(const std::string& url) noexcept -> Data
{
    auto doc  = download::Html(url).str();
    auto data = ParseData(utils::ExtractString(doc, from, to));

    data.website = Data::Website::Limetech;
    data.url     = url;

    return data;
}
}  // namespace smam
