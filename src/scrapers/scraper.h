#pragma once

#include "../downloader.h"

class Scraper{
	using cstr = const std::string;

protected:
	Downloader& downloader;
	cstr aptUrl, dataFrom, dataTo;

	Scraper(Downloader& downloader, cstr& url="", cstr& from="", cstr& to="");
	virtual ~Scraper();

public:
	// return proper name of target (resolve version wildcard)
	virtual std::string getFileName(cstr& name) const = 0;

	// return download url of a target
	virtual std::string getFileUrl(cstr& name, char tag) const = 0;

	// fetch url contents
	virtual void fetch(const std::string& url) = 0;

	// returns whether this scraper is applicable for given url
	bool match(const std::string& url) const;
};
