#include "curlmock.h"

#include <fstream>
#include <sstream>

#ifndef NDELAY
#include <chrono>
#include <thread>
#endif

#include <map>
#include <vector>

#ifdef _PROJECT_ROOT
#define PROJECT_ROOT _PROJECT_ROOT
#else
#define PROJECT_ROOT ".."
#endif

std::map<std::string, std::string> tpLink = {
#include "./mockdata/thirdparty_link.json"
};

static std::string errmsg;

auto fetchData(const std::string& url) noexcept -> std::vector<char>
{
    if (tpLink.count(url))  // call to thirdparty
    {
        std::string location =
            PROJECT_ROOT "/test/mockdata/thirdparty/" + tpLink[url];
        std::ifstream ifs(location, std::ios::in | std::ios::ate);
        if (!ifs.is_open()) return {};

        auto              len = ifs.tellg();
        std::vector<char> result(len);

        ifs.seekg(0, std::ios::beg);
        ifs.read(&result[0], len);
        ifs.close();

        return result;
    }
    else  // assume it's a database call
    {
        size_t pos = url.find('=');
        if (pos == (size_t)std::string::npos) return {};

        std::istringstream dataStream(url.substr(++pos));
        std::string        id, location;
        std::vector<char>  result{'['};

        while (std::getline(dataStream, id, ','))
        {
            location =
                PROJECT_ROOT "/test/mockdata/database/" + id + ".json";
            std::ifstream ifs(location, std::ios::in | std::ios::ate);
            if (!ifs.is_open()) continue;

            auto len      = ifs.tellg();
            auto prevSize = result.size();
            result.resize(prevSize + len + 1);  // +1 for comma

            ifs.seekg(0, std::ios::beg);
            ifs.read(&result[prevSize], len);
            ifs.close();

            result[result.size() - 1] = ',';
        }

        auto size = result.size();
        if (size == 1) return {};

        result[size - 1] = ']';  // turn last comma into ]
        return result;
    }
}

auto curl_easy_init() -> CURL* { return new CURL; }

void curl_easy_setopt(CURL* curl, CURLoption opt, const char* arg)
{
    if (opt == CURLOPT_URL) curl->url = arg;
}

void curl_easy_setopt(CURL* curl, CURLoption opt, CURLcallback callback)
{
    if (opt == CURLOPT_WRITEFUNCTION) curl->callback = callback;
}

void curl_easy_setopt(CURL* curl, CURLoption opt, void* arg)
{
    if (opt == CURLOPT_WRITEDATA) curl->arg = arg;
}

void curl_easy_setopt(CURL*, CURLoption, long) {}

auto curl_easy_perform(CURL* curl) -> CURLcode
{
    auto result = fetchData(curl->url);
    if (!result.size())
    {
        errmsg = "Not found: \"" + curl->url + '\"';
        return CURLE_NOT_OK;
    }

    auto bytesSent = 0;
    auto bytesLeft = result.size();
    auto perRecv   = bytesLeft / 10;

#ifdef NDELAY
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(1s);
#endif

    while (bytesLeft > 0)
    {
        if (perRecv > bytesLeft) perRecv = bytesLeft;

        char* buffer = result.data() + bytesSent;
        curl->callback(buffer, 1, perRecv, curl->arg);

        bytesSent += perRecv;
        bytesLeft -= perRecv;
    }

    return CURLE_OK;
}

auto curl_easy_strerror(CURLcode) -> const char*
{
    return errmsg.data();
}

void curl_easy_cleanup(CURL* curl) { delete curl; }
