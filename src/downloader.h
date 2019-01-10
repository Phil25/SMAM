#pragma once

#include <curl/curl.h>
#include <string>
#include <fstream>

typedef const std::string cstr;

class Downloader{
	static std::string buffer;
	static CURL* curl;
	static CURLcode res;
	static std::ofstream tempfile;

	Downloader() = delete;
	Downloader(const Downloader&) = delete;

public:
	static std::string html(cstr& url, cstr& from, cstr& to);
	static void file(cstr& url);

private:
	static bool prepare();
	static void set_opts(CURL*, cstr&);
	static size_t read(const char* contents, size_t size, size_t nmemb);
	static size_t write(const char* contents, size_t size, size_t nmemb);

	static std::string extract(cstr& from, cstr& to);
};
