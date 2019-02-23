#pragma once

#include <map>
#include <string>

#include "../src/downloader.h"

class DownloaderMock : public Downloader
{
	using cstr = const std::string;

public:
	// map linking url to file name containing the data
	static std::map<std::string, std::string> dataLink;

	// retrieves data from file, between sepcified values
	std::string html(cstr& url, cstr& from, cstr& to) override;

	// retrieves data from file, provided url found in dataLink
	static std::string getData(std::string url);

	// retrieves data from file
	static std::string readFile(std::string name);
};
