#include "downloader.h"
#include "utils.h"

#define AGENT "SourceMod Addon Manager"
#define VERSION "1.0"

Downloader::Downloader():
	buffer(""),
	curl(NULL),
	res(CURLE_FAILED_INIT),
	tempfile({})
{}

std::string Downloader::html(cstr& url, cstr& from, cstr& to){
	if(!prepare()) return "";
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, read);
	set_opts(curl, url);

	res = curl_easy_perform(curl);
	if(res != CURLE_OK)
		fprintf(stderr, "Error: %s\n", curl_easy_strerror(res));

	curl_easy_cleanup(curl);
	return Utils::extract(buffer, from, to);
}

void Downloader::file(cstr& url){
	if(!prepare()) return;
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write);
	set_opts(curl, url);

	tempfile.open("test.txt"); // TODO: get from URL
	res = curl_easy_perform(curl);
	if(res != CURLE_OK)
		fprintf(stderr, "Error: %s\n", curl_easy_strerror(res));

	tempfile.close();
	curl_easy_cleanup(curl);
}

bool Downloader::prepare(){
	buffer.clear();
	curl = curl_easy_init();
	return curl != NULL;
}

void Downloader::set_opts(CURL* curl, cstr& url){
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_USERAGENT, AGENT "/" VERSION);

	// pass pointer of the caller's class
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);

	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 4L);
}

size_t Downloader::read(const char* data, size_t size, size_t n, void* o){
	size_t trueSize = size *n;
	static_cast<Downloader*>(o)->buffer.append(data, trueSize);
	return trueSize;
}

size_t Downloader::write(const char* data, size_t size, size_t n, void* o){
	size_t trueSize = size *n;
	static_cast<Downloader*>(o)->tempfile << std::string(data, trueSize);
	return trueSize;
}
