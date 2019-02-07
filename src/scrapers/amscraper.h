#pragma once

#include <map>

#include "scraper.h"

using Attachments = std::map<std::string, std::string>;

class AMScraper : public Scraper{
	Attachments attachments;

public:
	AMScraper(Downloader&);
	virtual ~AMScraper();

	void fetch(const std::string& url) override;
	std::string getFileName(const std::string& name) const override;
	std::string getFileUrl(const std::string& name, char tag) const override;

private:
	// return name with the highest version number based on contents
	std::string getWildcard(const std::string& name, int at) const;
};
