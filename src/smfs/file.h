#pragma once

#include <filesystem>
#include <string>

#include <scrapers/scraper.h>

#include <nlohmann/json.hpp>

class File final
{
    std::string name, path, url;

public:
    File(const std::string& data) noexcept;

    auto raw() const noexcept -> std::string;

    auto getUrl() const noexcept -> std::string;

    /*
     * Evaluate file instance altering its fields based on data
     * retrieved from its origin website by a scraper, which can be an
     * empty object.
     *
     * The purpose is to obtain a File having appropriate `name`
     * (instead of ex. a link or regex string), and `url` (URL from
     * which the file can be downloaded).
     *
     * In most situations only `url` gets altered. In case if `name`
     * contains a link or regex, both fields are altered.
     *
     * This function will do the following for this instance:
     *
     * - Check if `name` is found in Attachments.
     *   If so, it is assumed that the name is correct. URL is set.
     *
     * - Check if `name` is a link.
     *   If so, it is assumed that it ends in the actual file name.
     *   Name and URL are set.
     *
     * - Check if matches in Attachments can be found when `name is
     *   treated as a regex string.
     *   If multiple matches are found, it is assumed that Attachments
     *   consist of similar files with a version number in the name of
     *   each (file-2.3.txt, file-2.4.txt, file-2.4.1.txt, etc...). Name
     *   is set to the one containing the largest version number. URL is
     *   set to the Attachment URL.
     *
     * - Otherwise (the fallback case) it is assumed that the File
     *   download URL can be obtained by combining the addon's base URL
     *   and the File name. URL is set.
     */
    void evaluate(const Scraper::Data&) noexcept;

    /*
     * Implicit conversion to std::filesystem::path
     */
    operator std::filesystem::path() const;

    bool operator==(const File& other) const noexcept;
};

namespace nlohmann
{
template <>
struct adl_serializer<File>
{
    static auto from_json(const nlohmann::json& j) noexcept -> File;
    static void to_json(json& j, const File&) noexcept;
};
}  // namespace nlohmann
