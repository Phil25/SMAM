#pragma once

#include <string>
#include <vector>
#include "scrapers/scraper.hpp"
#include "database.h"

class Plan{
	struct File{
		char tag;
		std::string dir,  // SM directory destination
					name, // name or relative path
					url = "";  // url to download from

		File(const std::string& data);
		static std::string toDir(char);
	};

	std::string id; // addon identifier
	std::string url = ""; // main url of the plan
	std::vector<File> files{};

public:
	Plan(const std::string&);

	void init(const Database&);
	void fetch(const Scraper&);

	int size() const;
	const std::string& getUrl() const;

	char getFileTag(int) const;
	const std::string& getFileDir(int) const;
	const std::string& getFileName(int) const;
	const std::string& getFileUrl(int) const;

	void setFileDir(int, const std::string&);
	void setFileName(int, const std::string&);
	void setFileUrl(int, const std::string&);
};
