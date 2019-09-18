#include "curl.h"

#include <fstream>
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

#define DATABASE_PATH "/test/mock/data/database/"
#define THRIDPARTY_PATH "/test/mock/data/thirdparty/"

const std::map<std::string, std::string> tpLink = {
#include "./data/thirdparty_link.json"
};

using json = nlohmann::json;

namespace
{
std::string errmsg;

inline void BuildResult(json& result, const std::string& id,
                        std::set<std::string>& ids) noexcept
{
    if (ids.count(id)) return;  // already added

    auto loc = PROJECT_ROOT DATABASE_PATH + id + ".json";

    auto ifs = std::ifstream(loc, std::ios::in);
    if (!ifs) return;

    auto j = json::parse(ifs);
    ifs.close();

    ids.insert(id);
    result.push_back(j);

    if (j.count("deps"))
    {
        for (const std::string& dep : j["deps"])
        {
            BuildResult(result, dep, ids);
        }
    }
}
}  // namespace

auto mockcurl::QueryDatabase(const std::string& url) noexcept
    -> std::vector<char>
{
    auto pos = url.find('=');
    if (pos == std::string::npos) return {};  // invalid db call

    auto data = std::istringstream(url.substr(++pos));
    auto ids  = std::set<std::string>();
    auto id   = std::string();
    auto res  = json::array();

    while (std::getline(data, id, ','))
    {
        BuildResult(res, id, ids);
    }

    auto str = res.dump();
    return {str.begin(), str.end()};
}

auto FetchData(const std::string& url) noexcept -> std::vector<char>
{
    if (!tpLink.count(url))  // unknown -> assume call to database
    {
        return mockcurl::QueryDatabase(url);
    }  // else call to a thirdparty website

    auto loc = PROJECT_ROOT THRIDPARTY_PATH + tpLink.at(url);

    auto ifs = std::ifstream(loc, std::ios::in | std::ios::ate);
    if (!ifs) return {};

    auto length = ifs.tellg();
    if (!length) return {};

    auto res = std::vector<char>(length);

    ifs.seekg(0, std::ios::beg);
    ifs.read(&res.front(), length);
    ifs.close();

    return res;
}

auto curl_easy_init() -> CURL*
{
    return new CURL;
}

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

void curl_easy_setopt(CURL*, CURLoption, long)
{
}

auto curl_easy_perform(CURL* curl) -> CURLcode
{
    auto result = FetchData(curl->url);
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

void curl_easy_cleanup(CURL* curl)
{
    delete curl;
}
