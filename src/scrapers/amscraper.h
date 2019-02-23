#pragma once

#include <memory>

#include "scraper.h"

class AMScraper : public Scraper
{
public:
	AMScraper(Downloader&);
	virtual ~AMScraper();

	Attachments fetch(const std::string& url) override;
};
