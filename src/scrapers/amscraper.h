#pragma once

#include "scraper.hpp"

class AMScraper : Scraper{

public:
	AMScraper(Downloader&);

	std::string getFileUrl(const std::string& name, char tag) const override;

private:
	// build url for downloading attachment
	std::string buildUrl(int attachmentId, bool forumCompilable) const;

	// extract forum's attachment id from line
	int getId(const std::string& line) const;


	// check if the file is a forum-compilable source
	bool isForumCompilable(char tag, const std::string& name) const;

	// check if name ends with ".sp"
	bool isFileSource(const std::string& name) const;
};
