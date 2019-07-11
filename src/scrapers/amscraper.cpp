#include "amscraper.h"

#include <cassert>

#include <download.h>
#include <utils/misc.h>
#include <utils/printer.h>
#include <utils/version.h>

#include <pugixml.hpp>

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
inline bool isUrlReplaceable(const std::string& url)
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

    AMNode(const pugi::xml_node& node) noexcept : node(node) {}

    auto name() const noexcept -> std::string
    {
        std::string name;
        auto        src = node.first_child();

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

            name = Utils::extract(src.value(), " (", ".sp - ");
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

            name = Utils::extract(src.value(), " (", " - ");
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

    auto url() const noexcept -> std::string
    {
        auto href = node.attribute("href");
        if (!href) return "";

        std::string url = href.as_string();

        if (!Utils::isLink(url)) url.insert(0, URL);

        return url;
    }

public:
    static auto parseData(const std::string& doc) noexcept;
};

auto AMNode::parseData(const std::string& doc) noexcept
{
    pugi::xml_document root;
    Scraper::Data      data;

    root.load_string(doc.c_str());

    for (const auto& anchor : root.select_nodes("//a"))
    {
        auto node  = AMNode(anchor.node());
        auto name  = node.name();
        auto url   = node.url();
        auto entry = data.find(name);

        if (entry == data.end())
        {
            data.emplace(name, url);
        }
        else if (isUrlReplaceable(entry->second))
        {
            entry->second = url;
        }
    }

    return data;
}
}  // namespace

AMScraper::AMScraper() noexcept
    : Scraper(URL, "<!-- attachments -->", "<!-- / attachments -->")
{
}

AMScraper::~AMScraper() noexcept = default;

auto AMScraper::fetch(const std::string& url) noexcept -> Data
{
    auto doc  = Download::page(url, dataFrom, dataTo);
    auto data = AMNode::parseData(doc);

    data.website = Data::Website::AlliedModders;

    return data;
}
