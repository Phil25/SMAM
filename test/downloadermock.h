#pragma once

#include <map>
#include <string>

#include "../src/downloader.h"

class DownloaderMock : public Downloader
{
	using cstr = const std::string;

public:
	// retrieves data from file, between sepcified values
	std::string html(cstr& url, cstr& from, cstr& to) override;
};
