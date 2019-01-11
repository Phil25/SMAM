#pragma once

#include <sstream>
#include <string>
#include <vector>

#include "../downloader.h"

class Scraper{
	using cstr = const std::string;
	using StringVector = std::vector<std::string>;

protected:
	Downloader& downloader;
	cstr aptUrl, dataFrom, dataTo;

	StringVector contents;

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

	// is this scraper applicable for given url
	bool match(const std::string& url) const{
		return url.compare(0, aptUrl.size(), aptUrl) == 0;
	}

	// download page and parse relevant data
	void getPage(const std::string& url){
		contents = toLines(downloader.html(url, dataFrom, dataTo));
	}

	// split string to vector by newlines
	static StringVector toLines(const std::string& data){
		std::istringstream dataStream(data);
		std::string line;
		std::vector<std::string> lines;

		while(std::getline(dataStream, line))
			lines.push_back(line);

		return lines;
	}
};
