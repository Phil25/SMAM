#pragma once

#ifdef NDEBUG
#include <curl/curl.h>
#else
#include "../test/curlmock.h"
#endif

#include <string>

class Downloader
{
    CURL* curl = NULL;

public:
    Downloader() noexcept;

    auto html(const std::string& url, const std::string& from = "",
              const std::string& to = "") noexcept -> std::string;

    auto file(const std::string& url, const std::string& dest) noexcept
        -> std::string;
};
