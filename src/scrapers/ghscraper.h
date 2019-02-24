#pragma once

#include "scraper.h"

class GHScraper : public Scraper
{
public:
	GHScraper(Downloader&);
	virtual ~GHScraper();

	Attachments fetch(const std::string& url) override;
};
