#include "file.h"

#include <regex>

#include <utils/misc.h>
#include <utils/version.h>

using json           = nlohmann::json;
using FileSerializer = nlohmann::adl_serializer<File>;

/*
 * Return vector of Attachment names matched against `base` regex.
 */
inline auto findMatches(const std::string&   base,
                        const Scraper::Data& data) noexcept
{
    std::vector<std::string> filtered;

    try
    {
        const std::regex re(base);

        for (const auto& [name, url] : data)
        {
            if (std::regex_match(name, re))
            {
                filtered.push_back(name);
            }
        }
    }
    catch (const std::regex_error& e)
    {
    }

    return filtered;
}

File::File(const std::string& data) noexcept
{
    auto at = data.find_first_of(';');

    if (at == std::string::npos) at = data.find_last_of('/');

    path = data.substr(0, at);
    name = data.substr(at + 1);
}

auto File::raw() const noexcept -> std::string
{
    return path + '/' + name;
}

auto File::getUrl() const noexcept -> std::string { return url; }

void File::evaluate(const Scraper::Data& data) noexcept
{
    auto attachment = data.find(name);

    if (attachment != data.end())  // found
    {
        url = attachment->second;
        return;
    }

    if (Utils::isLink(name))  // specified file is a link
    {
        size_t pos = name.rfind('/');
        if (pos != std::string::npos)
        {
            url  = name;
            name = name.substr(++pos);
        }

        return;
    }

    auto names = findMatches(name, data);

    if (!names.empty())
    {
        std::string name = Utils::Version::biggest(names);
        attachment       = data.find(name);

        if (attachment != data.end())
        {
            name = name;
            url  = attachment->second;

            return;
        }
    }

    if (data.website == Scraper::Data::Website::AlliedModders)
    {
        // file.invalidate(); TODO
    }
    else
    {
        url += name;
    }
}

File::operator std::filesystem::path() const { return raw(); }

bool File::operator==(const File& other) const noexcept
{
    return raw() == other.raw();
}

auto FileSerializer::from_json(const json& j) noexcept -> File
{
    return {j.get<std::string>()};
}

void FileSerializer::to_json(json& j, const File& file) noexcept
{
    j = file.raw();
}
