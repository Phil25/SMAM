#include "curlmock.h"

#include <fstream>
#include <map>
#include <set>
#include <sstream>

#ifndef NDELAY
#include <chrono>
#include <thread>
#endif

#include <nlohmann/json.hpp>

#ifdef _PROJECT_ROOT
#define PROJECT_ROOT _PROJECT_ROOT
#else
#define PROJECT_ROOT ".."
#endif

std::map<std::string, std::string> tpLink = {
#include "./mockdata/thirdparty_link.json"
};

using json = nlohmann::json;

namespace
{
std::string errmsg;

inline void buildResult(json& result, const std::string& id,
                        std::set<std::string>& ids) noexcept
{
    if (ids.count(id)) return;  // already added

    auto location =
        PROJECT_ROOT "/test/mockdata/database/" + id + ".json";

    std::ifstream ifs(location, std::ios::in);
    if (!ifs.is_open()) return;

    auto j = json::parse(ifs);
    ifs.close();

    ids.insert(id);
    result.push_back(j);

    if (j.count("deps"))
    {
        for (const std::string& dep : j["deps"])
        {
            buildResult(result, dep, ids);
        }
    }
}
}  // namespace

auto CURLMock::fetchData(const std::string& url) noexcept
    -> std::vector<char>
{
    if (!tpLink.count(url))  // unknown -> assume call to database
    {
        return queryDatabase(url);
    }

    // else call to a thirdparty website

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

auto CURLMock::queryDatabase(const std::string& url) noexcept
    -> std::vector<char>
{
    size_t pos = url.find('=');
    if (pos == (size_t)std::string::npos) return {'[', ']'};

    auto data   = std::istringstream(url.substr(++pos));
    auto ids    = std::set<std::string>();
    auto id     = std::string();
    auto result = json::array();

    while (std::getline(data, id, ','))
    {
        buildResult(result, id, ids);
    }

    auto str = result.dump();
    return std::vector<char>{str.begin(), str.end()};
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
    auto result = CURLMock::fetchData(curl->url);
    if (!result.size())
    {
        errmsg = "Not found: \"" + curl->url + '\"';
        return CURLE_NOT_OK;
    }

    auto bytesSent = 0;
    auto bytesLeft = result.size();
    auto perRecv   = bytesLeft / 10;

    if (perRecv < 10) perRecv = 10;

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
