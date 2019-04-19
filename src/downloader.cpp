#include <iostream>

#include "downloader.h"
#include "utils/misc.h"

#define AGENT "SourceMod Addon Manager"
#define VERSION "1.0"

Downloader::Downloader():
	curl(NULL)
{
}

/*
 * Download contents of a website and return it as std::string.
 * Passing nonempty `from` and `to` parameters returns only the data after
 * the first `from` match and before the following `to` match, excluding.
 */
std::string Downloader::html(cstr& url, cstr& from, cstr& to)
{
	if((curl = curl_easy_init()) == NULL)
	{
		return "";
	}

	std::string buffer;
	set_opts(url, read, &buffer);

	CURLcode res = curl_easy_perform(curl);
	if(res != CURLE_OK)
	{
		std::cerr << "Error: " << curl_easy_strerror(res) << '\n';
	}

	curl_easy_cleanup(curl);

	return Utils::extract(buffer, from, to);
}

/*
 * Download a file and write it to specified destination.
 */
bool Downloader::file(cstr& url, cstr& dest)
{
	if((curl = curl_easy_init()) == NULL)
	{
		return false;
	}

	std::ofstream tempfile(dest);
	set_opts(url, write, &tempfile);

	CURLcode res = curl_easy_perform(curl);
	bool success = res != CURLE_OK;

	if(!success)
	{
		fprintf(stderr, "Error: %s\n", curl_easy_strerror(res));
	}

	tempfile.close();
	curl_easy_cleanup(curl);

	return success;
}

/*
 * Set generic CURL options shared accross `html` and `file` calls.
 */
void Downloader::set_opts(cstr& url, curlcb callback, void* data)
{
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_USERAGENT, AGENT "/" VERSION);

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, data);

	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 4L);
}

/*
 * `read` is used by `html` to append the downloaded data to buffer.
 */
size_t Downloader::read(const char* data, size_t size, size_t n, void* b)
{
	size_t trueSize = size *n;
	std::string* pbuffer = static_cast<std::string*>(b);

	pbuffer->append(data, trueSize);

	return trueSize;
}

/*
 * `write` is used by `file` to pass the downloaded data to file stream.
 */
size_t Downloader::write(const char* data, size_t size, size_t n, void* f)
{
	size_t trueSize = size *n;
	std::ofstream* pfile = static_cast<std::ofstream*>(f);

	*pfile << std::string(data, trueSize);

	return trueSize;
}
