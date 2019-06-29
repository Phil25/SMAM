#include "ltscraper.h"

#include <libxml++/libxml++.h>

#include "../download.h"

#include "../utils/printer.h"

namespace
{
constexpr std::string_view URL     = "https://builds.limetech.io/";
int                        tdCount = 0;
bool                       found   = false;

/*
 * Is the <td> table node valid:
 *
 * - Is actually a <td>
 *
 * - Is it every 3rd node when iterating, starting from second;
 *   From a table of 3 columns, Linux versions are in the second one.
 *
 * - Does it contain an anchor <a> node as a child.
 *   Otherwise, it is assumed the contents are `none.`
 */
inline bool isValidTd(const xmlpp::Node* node)
{
    if (node->get_name() != "td")
    {
        return false;
    }

    if (++tdCount % 3 == 1)  // every third <td>
    {
        return false;
    }

    const auto child = node->get_first_child();
    return child ? child->get_name() == "a" : false;
}

/*
 * If a valid <td> node is found, save its name and download URL.
 */
inline void setValidTd(xmlpp::Node* node, std::string& name,
                       std::string& url)
{
    auto e =
        dynamic_cast<const xmlpp::Element*>(node->get_first_child());
    if (e)  // should always be true
    {
        url = e->get_attribute_value("href");

        size_t pos = url.rfind('/');
        if (pos != std::string::npos)
        {
            name = url.substr(++pos);
        }

        url.insert(0, URL);
    }
}

/*
 * Recursively iterate nodes to find a valid <td> node.
 * Stop iterating after the first valid one is found.
 */
inline void findValidTd(xmlpp::Node* node, std::string& name,
                        std::string& url)
{
    if (found) return;

    if (isValidTd(node))
    {
        setValidTd(node, name, url);
        found = true;
    }
    else
    {
        for (const auto child : node->get_children())
        {
            findValidTd(child, name, url);
        }
    }
}
}  // namespace

LTScraper::LTScraper() noexcept : Scraper(URL, "</h2>", "</div>") {}

LTScraper::~LTScraper() noexcept = default;

auto LTScraper::fetch(const std::string& url) noexcept -> Attachments
{
    xmlpp::DomParser contents;
    std::string      fileName, fileUrl;

    try
    {
        contents.parse_memory(Download::page(url, dataFrom, dataTo));
        auto root = contents.get_document()->get_root_node();
        tdCount   = 0;
        found     = false;

        findValidTd(root, fileName, fileUrl);
    }
    catch (const std::exception& e)
    {
        out(Ch::Error) << e.what() << cr;
    }

    return {{fileName, fileUrl}};
}
