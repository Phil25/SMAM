#pragma once

#include <curl/curl.h>
#include <string>
#include <fstream>

class Downloader{
	using cstr = const std::string;

	std::string buffer;
	CURL* curl;
	CURLcode res;
	std::ofstream tempfile;

public:
	Downloader();

	virtual std::string html(cstr& url, cstr& from, cstr& to);
	virtual void file(cstr& url);

private:
	bool prepare();
	void set_opts(CURL*, cstr& url);

	static size_t read(const char*, size_t size, size_t n, void*);
	static size_t write(const char*, size_t size, size_t n, void*);
};
