#include "amscraper.h"

#include <net/download.h>
#include <utils/common.h>

#include <pugixml.hpp>

#include <cassert>

namespace
{
constexpr std::string_view URL     = "https://forums.alliedmods.net/";
constexpr std::string_view URL_ALT = "http://www.sourcemod.net/";

/*
 * Is the URL replacable (URL_ALT).
 * URL_ALT specifies a URL for a source file (.sp) that is expected
 * to be compilible with the forum compiler upon downloading. Not all
 * plugins can be compiled with the forum compiler, which is why
 * a precompiled plugin file (.smx) is usually uploaded alongside.
 *
 * If two of the same file names (example.smx) are found, it is assumed
 * that the one with NONREPLACABLE URL is the precompiled one, taking
 * priority over the REPLACABLE one.
 */
inline bool IsUrlReplaceable(const std::string& url) noexcept
{
    return url.compare(0, URL_ALT.size(), URL_ALT) == 0;
}

/*
 * Wrapper around pugi::xml_node for dealing with finding the
 * attachment's name and its download URL.
 */
class AMNode final
{
    const pugi::xml_node& node;

    AMNode(const pugi::xml_node& node) noexcept : node(node)
    {
    }

    auto Name() const noexcept -> std::string
    {
        namespace utils = smam::utils;

        auto name = std::string{};
        auto src  = node.first_child();

        if (!node.text())
        {
            /*
             * If node is not a text node, this means it contains tags
             * within, which are "<strong>Get Plugin</strong>". This
             * indicates that the actual filename is 3 nodes over in
             * parentheses.
             */

            src = node.next_sibling().next_sibling().next_sibling();

            assert(src.text() && "Node should be text");

            name = utils::ExtractString(src.value(), " (", ".sp - ");
            name.append(".smx");
        }
        else if (std::string{src.value()} == "Get Source")
        {
            /*
             * If child node value is "Get Source", it's at the same
             * attachment as the above condition, but only a single jump
             * away from the actual filename.
             */

            src = node.next_sibling();

            assert(src.text() && "Node should be text");

            name = utils::ExtractString(src.value(), " (", " - ");
        }
        else
        {
            /*
             * Otherwise the actual filename is the node's value.
             */

            name = src.value();
        }

        return name;
    }

    auto Url() const noexcept -> std::string
    {
        namespace utils = smam::utils;

        auto href = node.attribute("href");
        if (!href) return "";

        std::string url = href.as_string();

        if (!utils::IsLink(url)) url.insert(0, URL);

        return url;
    }

public:
    static auto ParseData(const std::string& doc) noexcept;
};

auto AMNode::ParseData(const std::string& doc) noexcept
{
    auto root = pugi::xml_document{};
    auto data = smam::Scraper::Data{};

    root.load_string(doc.c_str());

    for (const auto& anchor : root.select_nodes("//a"))
    {
        auto node  = AMNode(anchor.node());
        auto name  = node.Name();
        auto url   = node.Url();
        auto entry = data.nameToLink.find(name);

        if (entry == data.nameToLink.end())
        {
            data.nameToLink.emplace(name, url);
        }
        else if (IsUrlReplaceable(entry->second))
        {
            entry->second = url;
        }
    }

    return data;
}
}  // namespace

namespace smam
{
AMScraper::AMScraper() noexcept
    : Scraper(URL, "<!-- attachments -->", "<!-- / attachments -->")
{
}

AMScraper::~AMScraper() noexcept = default;

auto AMScraper::Parse(const std::string& url) noexcept -> Data
{
    auto doc  = download::Html(url).str();
    auto data = AMNode::ParseData(utils::ExtractString(doc, from, to));

    data.website = Data::Website::AlliedModders;

    return data;
}
}  // namespace smam
