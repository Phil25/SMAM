#pragma once

#include "../plan.h"

class Scraper{

protected:
	std::string url;

	Scraper(const std::string& url) : url(url){}

public:
	virtual void completePlan(Plan&) const = 0;

	bool match(const std::string& otherUrl) const{
		return otherUrl.compare(0, url.size(), url) == 0;
	}
};
