#pragma once

#ifdef NDEBUG
#include <curl/curl.h>
#else
#include "../test/curlmock.h"
#endif

#include <string>

class Downloader
{
	using cstr = const std::string;
	CURL* curl = NULL;

public:
	Downloader();

	std::string html(cstr& url, cstr& from="", cstr& to="");
	bool file(cstr& url, cstr& dest);
};
