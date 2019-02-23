#pragma once

#include "scraper.h"

class LTScraper : public Scraper
{
public:
	LTScraper(Downloader&);
	virtual ~LTScraper();

	Attachments fetch(const std::string& url) override;
};
