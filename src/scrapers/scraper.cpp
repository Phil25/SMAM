#include "scraper.h"

Scraper::Scraper(Downloader& downloader, cstr& url, cstr& from, cstr& to):
	downloader(downloader),
	aptUrl(url),
	dataFrom(from),
	dataTo(to)
{}

Scraper::~Scraper(){}

bool Scraper::match(const std::string& url) const{
	return url.compare(0, aptUrl.size(), aptUrl) == 0;
}

void Scraper::getPage(const std::string& url){
	contents = Utils::toLines(downloader.html(url, dataFrom, dataTo));
}
