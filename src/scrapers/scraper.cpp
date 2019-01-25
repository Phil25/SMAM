#include "scraper.h"

Scraper::Scraper(Downloader& downloader, cstr& url, cstr& from, cstr& to):
	downloader(downloader),
	aptUrl(url),
	dataFrom(from),
	dataTo(to)
{
	scrapers.push_back(this);
}

Scraper::~Scraper(){}

const Scraper* Scraper::get(const std::string& url){
	for(const Scraper* scraper : scrapers)
		if(scraper->match(url))
			return scraper;
	return nullptr;
}

bool Scraper::match(const std::string& url) const{
	return url.compare(0, aptUrl.size(), aptUrl) == 0;
}

void Scraper::getPage(const std::string& url){
	contents = Utils::toLines(downloader.html(url, dataFrom, dataTo));
}

std::vector<Scraper*> Scraper::scrapers = {};
