#pragma once

#include <map>
#include <memory>
#include <vector>

#include "../downloader.h"

using Attachments = std::map<std::string, std::string>;

class Scraper
{
	using cstr = const std::string;
	using strv = const std::string_view;

protected:
	Downloader& downloader;
	cstr aptUrl, dataFrom, dataTo;

	Scraper(Downloader& downloader, strv url, cstr& from="", cstr& to="");
	virtual ~Scraper();

public:
	// fetch file download urls
	virtual Attachments fetch(const std::string& url) = 0;

	// returns whether this scraper is applicable for given url
	bool match(const std::string& url) const;
};
