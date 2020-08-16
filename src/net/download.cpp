#include "download.h"

#include <fstream>
#include <sstream>

#ifdef ISOLATED
#include <algorithm>
#endif

#include <curl/curl.h>
#include <utils/common.h>
#include <version.hpp>

namespace
{
/*
 * Write downloaded data into an ostream
 */
size_t Write(const char* data, size_t sz, size_t n, void* out) noexcept
{
    size_t trueSize = sz * n;
    static_cast<std::ostream*>(out)->write(data, trueSize);
    return trueSize;
}

#ifdef ISOLATED
/*
 * When isolated, a call to url X is forwarded to localhost/?external=X
 * X must contain no '&' chars to not confuse the GET param list
 */
auto SanitizeParam(std::string param) noexcept
{
    std::replace(param.begin(), param.end(), '&', '-');
    return param;
}
#endif  // ISOLATED
}  // namespace

namespace smam
{
download::Error::operator bool() const noexcept
{
    return !message.empty();
}

auto download::Raw(const std::string& url, std::ostream& os) noexcept
    -> Error
{
    auto* curl = curl_easy_init();
    if (curl == NULL)
    {
        return {"Download initialization failed."};
    }

#ifdef ISOLATED
    auto prefix = std::string{"http://localhost:2333/?external="};
    prefix.append(SanitizeParam(url));
    curl_easy_setopt(curl, CURLOPT_URL, prefix.c_str());
#else
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
#endif  // ISOLATED

    curl_easy_setopt(curl, CURLOPT_USERAGENT, version::FullAgent());

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Write);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &os);

    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

    auto code = curl_easy_perform(curl);
    if (code != CURLE_OK)
    {
        return {curl_easy_strerror(code)};
    }

    curl_easy_cleanup(curl);

    return {};
}

auto download::Html(const std::string& url) noexcept
    -> std::stringstream
{
    auto oss = std::stringstream();
    if (auto error = download::Raw(url, oss)) return {};
    return oss;
}

auto download::File(const std::string& url, const char* dest) noexcept
    -> Error
{
    auto ofs = std::ofstream(dest);
    if (auto error = download::Raw(url, ofs)) return error;
    return {};
}
}  // namespace smam
