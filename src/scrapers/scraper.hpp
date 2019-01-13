#pragma once

#include <sstream>
#include <string>
#include <vector>

#include "../utils.h"
#include "../downloader.h"

class Scraper{
	using cstr = const std::string;

protected:
	Downloader& downloader;
	cstr aptUrl, dataFrom, dataTo;

	Utils::StringVector contents;

	Scraper(Downloader& downloader, cstr& url="", cstr& from="", cstr& to=""):
		downloader(downloader),
		aptUrl(url),
		dataFrom(from),
		dataTo(to)
	{}

	virtual ~Scraper(){}

public:
	// return download url of a target
	virtual std::string getFileUrl(cstr& name, char tag) const = 0;

	// returns whether this scraper is applicable for given url
	bool match(const std::string& url) const{
		return url.compare(0, aptUrl.size(), aptUrl) == 0;
	}

	// download page and parse relevant data
	void getPage(const std::string& url){
		contents = Utils::toLines(downloader.html(url, dataFrom, dataTo));
	}
};
