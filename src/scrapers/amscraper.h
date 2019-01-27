#pragma once

#include "scraper.h"

class AMScraper : public Scraper{

public:
	AMScraper(Downloader&);
	virtual ~AMScraper();

	std::string getFileName(const std::string& name) const override;
	std::string getFileUrl(const std::string& name, char tag) const override;

private:
	// return name with the highest version number based on contents
	std::string getWildcard(const std::string& name, int at) const;

	// build url for downloading attachment
	static std::string buildUrl(int attachmentId, bool forumCompilable);

	// extract forum's attachment id from line
	static int getId(const std::string& line);

	// check if the file is a forum-compilable source
	static bool isForumCompilable(char tag, const std::string& name);

	// check if name ends with ".sp"
	static bool isFileSource(const std::string& name);
};
