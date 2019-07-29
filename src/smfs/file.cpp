#include "file.h"

#include <regex>

#include <utils/misc.h>
#include <utils/version.h>

using json = nlohmann::json;

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

        for (const auto& [name, _] : data)
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

    if (path.at(path.size() - 1) == '/') path.pop_back();
}

auto File::getPath() const noexcept -> std::string { return path; }

auto File::getName() const noexcept -> std::string { return name; }

auto File::getUrl() const noexcept -> std::string { return url; }

auto File::raw() const noexcept -> std::string
{
    return path + '/' + name;
}

bool File::evaluate(const Scraper::Data& data) noexcept
{
    auto attachment = data.find(name);

    if (attachment != data.end())  // found
    {
        url = attachment->second;
        return true;
    }

    if (Utils::isLink(name))  // specified file is a link
    {
        size_t pos = name.rfind('/');
        if (pos != std::string::npos)
        {
            url  = name;
            name = name.substr(++pos);
        }

        return true;
    }

    auto names = findMatches(name, data);

    if (!names.empty())
    {
        std::string best = Utils::Version::biggest(names);
        attachment       = data.find(best);

        if (attachment != data.end())
        {
            name = best;
            url  = attachment->second;

            return true;
        }
    }

    if (data.website == Scraper::Data::Website::AlliedModders)
    {
        return false;
    }

    url += name;
    return true;
}

File::operator std::filesystem::path() const { return raw(); }

bool operator==(const File::Ptr& f1, const File::Ptr& f2) noexcept
{
    return f1->raw() == f2->raw();
}

bool operator!=(const File::Ptr& f1, const File::Ptr& f2) noexcept
{
    return !operator==(f1, f2);
}

void from_json(const json& j, File::Ptr& file)
{
    file = std::make_shared<File>(j);
}

void to_json(json& j, const File::Ptr& file) noexcept
{
    j = file->raw();
}
