#include "downloader.h"

#define AGENT "SourceMod Addon Manager"
#define VERSION "1.0"

std::string Downloader::buffer = "";
CURL* Downloader::curl = NULL;
CURLcode Downloader::res = CURLE_FAILED_INIT;
std::ofstream Downloader::tempfile;

std::string Downloader::html(cstr& url, cstr& from, cstr& to){
	if(!prepare()) return "";
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, read);
	set_opts(curl, url);

	res = curl_easy_perform(curl);
	if(res != CURLE_OK)
		fprintf(stderr, "Error: %s\n", curl_easy_strerror(res));

	curl_easy_cleanup(curl);
	return extract(from, to);
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

	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 4L);
}

size_t Downloader::read(const char *contents, size_t size, size_t nmemb){
	size_t trueSize = size *nmemb;
	buffer.append(contents, trueSize);
	return trueSize;
}

size_t Downloader::write(const char *contents, size_t size, size_t nmemb){
	size_t trueSize = size *nmemb;
	tempfile << std::string(contents, trueSize);
	return trueSize;
}

std::string Downloader::extract(cstr& from, cstr& to){
	size_t begin = buffer.find(from, 0) +from.size();
	size_t end = buffer.find(to, begin);
	return buffer.substr(begin, end -begin);
}
