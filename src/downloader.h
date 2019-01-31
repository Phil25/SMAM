#pragma once

#include <curl/curl.h>
#include <string>
#include <fstream>

class Downloader{
	using cstr = const std::string;
	typedef size_t (*curlcb)(const char*, size_t, size_t, void*);

	CURL* curl;

public:
	Downloader();

	virtual std::string html(cstr& url, cstr& from, cstr& to);
	virtual bool file(cstr& url, cstr& dest);

private:
	void set_opts(cstr& url, curlcb callback, void* data);

	static size_t read(const char*, size_t size, size_t n, void*);
	static size_t write(const char*, size_t size, size_t n, void*);
};
