#include <libxml++/libxml++.h>
#include <iostream>

#include "ltscraper.h"

static constexpr std::string_view URL = "https://builds.limetech.io/";
static int tdCount = 0;
static bool found = false;

bool isValidTd(const xmlpp::Node* node)
{
	if(node->get_name() != "td")
	{
		return false;
	}

	if(++tdCount % 3 == 1) // every third <td>
	{
		return false;
	}

	const auto child = node->get_first_child();
	return child ? child->get_name() == "a" : false;
}

void setValidTd(xmlpp::Node* node, std::string& name, std::string& url)
{
	auto e = dynamic_cast<const xmlpp::Element*>(node->get_first_child());
	if(e) // should always be true
	{
		url = e->get_attribute_value("href");

		size_t pos = url.rfind('/');
		if(pos != std::string::npos)
		{
			name = url.substr(++pos);
		}

		url.insert(0, URL);
	}
}

void findValidTd(xmlpp::Node* node, std::string& name, std::string& url)
{
	if(found) return;

	if(isValidTd(node))
	{
		setValidTd(node, name, url);
		found = true;
	}
	else
	{
		for(const auto child : node->get_children())
		{
			findValidTd(child, name, url);
		}
	}
}

LTScraper::LTScraper(Downloader& downloader):
	Scraper(downloader,
		URL,
		"</h2>",
		"</div>"
	)
{
}

LTScraper::~LTScraper()
{
}

Attachments LTScraper::fetch(const std::string& url)
{
	xmlpp::DomParser contents;
	std::string fileName, fileUrl;

	try
	{
		contents.parse_memory(downloader.html(url, dataFrom, dataTo));
		auto root = contents.get_document()->get_root_node();
		tdCount = 0;
		found = false;

		findValidTd(root, fileName, fileUrl);
	}
	catch(const std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}

	return {{fileName, fileUrl}};
}
