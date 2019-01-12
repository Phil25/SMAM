#pragma once

#include "scraper.hpp"

class AMScraper : public Scraper{

public:
	AMScraper(Downloader&);
	virtual ~AMScraper();

	std::string getFileUrl(const std::string& name, char tag) const override;

private:
	// build url for downloading attachment
	static std::string buildUrl(int attachmentId, bool forumCompilable);

	// extract forum's attachment id from line
	static int getId(const std::string& line);

	// check if the file is a forum-compilable source
	static bool isForumCompilable(char tag, const std::string& name);

	// check if name ends with ".sp"
	static bool isFileSource(const std::string& name);
};
