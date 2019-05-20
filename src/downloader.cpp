#include "version.hpp"
#include "downloader.h"

#include <fstream>
#include <sstream>

#include "utils/misc.h"

/*
 * Write downloaded data into an ostream
 */
size_t write(const char* data, size_t size, size_t n, void* voss)
{
	size_t trueSize = size * n;
	static_cast<std::ostream*>(voss)->write(data, trueSize);
	return trueSize;
}

/*
 * Set generic CURL options shared accross `html` and `file` calls.
 */
void setOpts(CURL* curl, const std::string& url, std::ostream* data)
{
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_USERAGENT, Version::fullAgent());

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, data);

	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 4L);
}

Downloader::Downloader()
{
}

/*
 * Download contents of a website and return it as std::string.
 * Passing nonempty `from` and `to` parameters returns only the data
 * after the first `from` match and before the following `to` match,
 * excluding.
 */
std::string Downloader::html(cstr& url, cstr& from, cstr& to)
{
	if((curl = curl_easy_init()) == NULL)
	{
		return {};
	}

	std::ostringstream oss;
	setOpts(curl, url, &oss);

	CURLcode res = curl_easy_perform(curl);
	if(res != CURLE_OK)
	{
		return {};
	}

	curl_easy_cleanup(curl);

	return Utils::extract(oss.str(), from, to);
}

/*
 * Download a file and write it to specified destination.
 */
auto Downloader::file(cstr& url, cstr& dest) -> std::string
{
	if((curl = curl_easy_init()) == NULL)
	{
		return "Failed initializing downloader";
	}

	std::ostringstream oss;
	setOpts(curl, url, &oss);

	CURLcode res = curl_easy_perform(curl);
	if(res != CURLE_OK)
	{
		return curl_easy_strerror(res);
	}

	std::ofstream ofs(dest);
	ofs << oss.str();

	ofs.close();
	curl_easy_cleanup(curl);

	return {};
}
