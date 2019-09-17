#include <string>
#include <vector>

enum CURLoption
{
    CURLOPT_URL,
    CURLOPT_USERAGENT,
    CURLOPT_WRITEFUNCTION,
    CURLOPT_WRITEDATA,
    CURLOPT_FOLLOWLOCATION,
    CURLOPT_TIMEOUT
};

enum CURLcode
{
    CURLE_OK,
    CURLE_NOT_OK
};

using CURLcallback = size_t (*)(const char*, size_t, size_t, void*);

struct CURL
{
    std::string  url;
    CURLcallback callback;
    void*        arg;
};

CURL* curl_easy_init();

void curl_easy_setopt(CURL*, CURLoption, const char*);
void curl_easy_setopt(CURL*, CURLoption, CURLcallback);
void curl_easy_setopt(CURL*, CURLoption, void*);
void curl_easy_setopt(CURL*, CURLoption, long);

CURLcode curl_easy_perform(CURL*);

const char* curl_easy_strerror(CURLcode);

void curl_easy_cleanup(CURL*);

namespace mockcurl
{
auto QueryDatabase(const std::string& url) noexcept
    -> std::vector<char>;
}
