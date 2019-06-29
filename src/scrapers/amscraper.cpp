#include "amscraper.h"

#include <libxml++/libxml++.h>

#include "../download.h"

#include "../utils/printer.h"
#include "../utils/version.h"

namespace
{
constexpr std::string_view URL     = "https://forums.alliedmods.net/";
constexpr std::string_view URL_ALT = "http://www.sourcemod.net/";
}  // namespace

/*
 * Wrapper around xmlpp::Node* for dealing with dynamic_casting
 * and validating relevant nodes, namely anchor nodes.
 */
class AMNode
{
    const xmlpp::Node*        node;
    const std::vector<AMNode> children;
    const std::string         url;

public:
    AMNode(const xmlpp::Node* node) noexcept
        : node(node), children(fetchChildren()), url(fetchUrl())
    {
    }

private:
    auto fetchChildren() const noexcept -> std::vector<AMNode>
    {
        std::vector<AMNode> nodes;

        for (const auto child : node->get_children())
        {
            nodes.push_back(AMNode(child));
        }

        return nodes;
    }

    auto fetchUrl() const noexcept -> std::string
    {
        auto e = dynamic_cast<const xmlpp::Element*>(node);
        if (!e) return "";

        std::string url = e->get_attribute_value("href");

        if (!Utils::isLink(url))
        {
            url.insert(0, URL);
        }

        return url;
    }

public:
    bool isAnchor() const noexcept { return node->get_name() == "a"; }

    auto getName() const noexcept -> std::string
    {
        std::string name;
        auto        sourceNode = node->get_first_child();
        auto t = dynamic_cast<const xmlpp::TextNode*>(sourceNode);

        if (!t)  // t->get_content() is "<strong>Get Plugin</strong>"
        {
            sourceNode =
                node->get_next_sibling()  // hop over to TextNode
                    ->get_next_sibling()  // contianing filename
                    ->get_next_sibling();

            t = dynamic_cast<const xmlpp::TextNode*>(sourceNode);

            if (t)  // should always be true
            {
                name = Utils::extract(t->get_content(), " (", ".sp - ");
                name.append(".smx");
            }
        }
        else if (t->get_content() == "Get Source")
        {
            sourceNode = node->get_next_sibling();
            t = dynamic_cast<const xmlpp::TextNode*>(sourceNode);

            if (t)  // should always be true
            {
                name = Utils::extract(t->get_content(), " (", " - ");
            }
        }
        else  // t->get_content() is actual name of attachment
        {
            name = t->get_content();
        }

        return name;
    }

    auto getUrl() const noexcept -> const std::string& { return url; }

    auto getChildren() const noexcept -> const std::vector<AMNode>&
    {
        return children;
    }
};

namespace
{
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
 * Recursively iterate all the nodes to get to anchor <a> nodes.
 * Fill the `map` paramater with file names, and file download URLs.
 */
inline void populateAttachments(const AMNode&         node,
                                Scraper::Attachments& map)
{
    if (node.isAnchor())
    {
        std::string name  = node.getName();
        auto        entry = map.find(name);

        if (entry == map.end())  // not found
        {
            map[name] = node.getUrl();
        }
        else if (isUrlReplaceable(entry->second))
        {
            entry->second = node.getUrl();
        }
    }
    else
    {
        for (const auto& child : node.getChildren())
        {
            populateAttachments(child, map);
        }
    }
}

inline auto fetchAttachments(const xmlpp::DomParser& parser)
    -> Scraper::Attachments
{
    const AMNode root = AMNode(parser.get_document()->get_root_node());

    Scraper::Attachments attachments;
    populateAttachments(root, attachments);  // recursive

    return attachments;
}
}  // namespace

AMScraper::AMScraper() noexcept
    : Scraper(URL, "<!-- attachments -->", "<!-- / attachments -->")
{
}

AMScraper::~AMScraper() noexcept = default;

auto AMScraper::fetch(const std::string& url) noexcept -> Attachments
{
    xmlpp::DomParser contents;

    try
    {
        contents.parse_memory(Download::page(url, dataFrom, dataTo));
    }
    catch (const std::exception& e)
    {
        out(Ch::Error) << e.what() << cr;
    }

    return fetchAttachments(contents);
}
