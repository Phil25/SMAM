#pragma once

#include "../utils.h"
#include "../downloader.h"

class Scraper{
	using cstr = const std::string;

	static std::vector<Scraper*> scrapers;

protected:
	Downloader& downloader;
	cstr aptUrl, dataFrom, dataTo;

	Utils::StringVector contents;

	Scraper(Downloader& downloader, cstr& url="", cstr& from="", cstr& to="");
	virtual ~Scraper();

public:
	// return download url of a target
	virtual std::string getFileUrl(cstr& name, char tag) const = 0;

	// get scrapper appropriate for the url, nullptr if none found
	static const Scraper* get(const std::string& url);

	// returns whether this scraper is applicable for given url
	inline bool match(const std::string& url) const;

	// download page and parse relevant data
	void getPage(const std::string& url);
};
