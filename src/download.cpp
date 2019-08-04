#include "download.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>

#ifdef NDEBUG
#include <curl/curl.h>
#else
#include <curlmock.h>
#endif

#include <utils/misc.h>
#include <version.hpp>

namespace
{
std::vector<std::pair<std::string, std::string>> downloadedFiles;
constexpr auto opts = std::filesystem::copy_options::overwrite_existing;

inline auto tempDir() noexcept
{
    namespace fs     = std::filesystem;
    static auto temp = fs::temp_directory_path() / "smam";

    if (!fs::exists(temp)) fs::create_directories(temp);

    return temp;
}

/*
 * Write downloaded data into an ostream
 */
inline size_t write(const char* data, size_t size, size_t n,
                    void* voidoss) noexcept
{
    size_t trueSize = size * n;
    static_cast<std::ostream*>(voidoss)->write(data, trueSize);
    return trueSize;
}

/*
 * Set generic CURL options shared accross `page` and `file` calls.
 */
inline void setOpts(CURL* curl, const std::string& url,
                    std::ostream* data) noexcept
{
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_USERAGENT, Version::fullAgent());

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, data);

    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 4L);
}
}  // namespace

/*
 * Download contents of a website and return it as std::string.
 * Passing nonempty `from` and `to` parameters returns only the data
 * after the first `from` match and before the following `to` match,
 * excluding.
 */
auto Download::page(const std::string& url, const std::string& from,
                    const std::string& to) noexcept -> std::string
{
    CURL* curl = curl_easy_init();
    if (curl == NULL)
    {
        return {};
    }

    std::ostringstream oss;
    setOpts(curl, url, &oss);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        return {};
    }

    curl_easy_cleanup(curl);

    return Utils::extract(oss.str(), from, to);
}

/*
 * Download a file and write it to specified destination.
 */
auto Download::file(const std::string& url,
                    const std::string& dest) noexcept -> std::string
{
    CURL* curl = curl_easy_init();
    if (curl == NULL)
    {
        return "Failed initializing downloader.";
    }

    std::ostringstream oss;
    setOpts(curl, url, &oss);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        return curl_easy_strerror(res);
    }

    auto name = dest;
    std::replace(name.begin(), name.end(), '/', '-');
    std::replace(name.begin(), name.end(), '.', 'a');

    std::ofstream ofs(tempDir() / name);
    ofs << oss.str();

    downloadedFiles.emplace_back(name, dest);

    ofs.close();
    curl_easy_cleanup(curl);

    return {};
}

void Download::placeFiles() noexcept
{
    namespace fs = std::filesystem;
    auto temp    = tempDir();

    for (const auto& [file, dest] : downloadedFiles)
    {
        auto path = temp / file;
        if (fs::exists(path))
        {
            fs::copy_file(path, dest, opts);
        }
    }

    fs::remove_all(temp);
    downloadedFiles.clear();
}
