#include "ltscraper.h"

#include <pugixml.hpp>

#include "../download.h"

#include "../utils/printer.h"

namespace
{
constexpr std::string_view URL = "https://builds.limetech.io/";

auto parseData(const std::string& doc) noexcept
{
    pugi::xml_document root;
    Scraper::Data      data;
    std::string        name, url;

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

    data.emplace(name, url);
    return data;
}
}  // namespace

LTScraper::LTScraper() noexcept : Scraper(URL, "<tbody>", "</tbody>") {}

LTScraper::~LTScraper() noexcept = default;

auto LTScraper::fetch(const std::string& url) noexcept -> Data
{
    auto doc  = Download::page(url, dataFrom, dataTo);
    auto data = parseData(doc);

    data.website = Data::Website::Limetech;

    return data;
}
