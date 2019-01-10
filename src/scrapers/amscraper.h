#pragma once

#include "scraper.hpp"

using StringVector = std::vector<std::string>;

class AMScraper : Scraper{

public:
	AMScraper();

	void completePlan(Plan&) const;

private:
	// split string to vector by newlines
	StringVector toLines(const std::string&) const;


	// get download url of attachment, using forum HTML data
	std::string getUrl(const std::string&, char, const StringVector&) const;

	// build url for attachment download
	std::string buildUrl(int attachmentId, bool forumCompilable) const;

	// extract forum's attachment id from line
	int getId(const std::string& line) const;


	// check if the file is a forum-compilable source
	bool isForumCompilable(char tag, const std::string& name) const;

	// check if name ends with ".sp"
	bool isFileSource(const std::string& name) const;
};
