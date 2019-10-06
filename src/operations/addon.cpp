#include "addon.h"

#include <net/download.h>
#include <utils/common.h>
#include <utils/path.h>

#include <regex>

namespace smam
{
namespace
{
bool Match(const std::string& url, const std::string& aptUrl) noexcept
{
    return url.compare(0, aptUrl.size(), aptUrl) == 0;
}

/*
 * Return vector of Attachment names matched against `base` regex.
 */
auto FindMatches(const std::string&   base,
                 const Scraper::Data& data) noexcept
{
    std::vector<std::string> filtered;

    try
    {
        const std::regex re(base);

        for (const auto& [name, _] : data.nameToLink)
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
}  // namespace

AddonContext::AddonContext(const AddonPtr& addon) noexcept
    : addon(addon)
{
}

FindData::FindData(
    const LoggerPtr& logger, AddonContext& context,
    const std::shared_ptr<ScraperArray>& scrapers) noexcept
    : Operation(logger, context), scrapers(scrapers)
{
}

void FindData::Run() noexcept
{
    const auto& baseUrl = GetContext().addon->BaseURL();

    for (const auto& scraper : *scrapers)
    {
        if (Match(baseUrl, scraper->Url()))
        {
            GetContext().data = scraper->Parse(baseUrl);
            return;
        }
    }
}

EvaluateFiles::EvaluateFiles(const LoggerPtr& logger,
                             AddonContext&    context) noexcept
    : Operation(logger, context)
{
}

void EvaluateFiles::Run() noexcept
{
    const auto& data = GetContext().data;

    for (auto& file : GetContext().addon->Files())
    {
        auto attachment = data.nameToLink.find(file->Name());

        if (attachment != data.nameToLink.end())  // found
        {
            file->Link(attachment->second);
            continue;
        }

        if (utils::IsLink(file->Name()))  // specified file is a link
        {
            size_t pos = file->Name().rfind('/');
            if (pos != std::string::npos)
            {
                file->Link(file->Name());
                file->Name(file->Name().substr(++pos));
            }

            continue;
        }

        auto names = FindMatches(file->Name(), data);

        if (!names.empty())
        {
            std::string best = utils::BiggestVersion(names);
            attachment       = data.nameToLink.find(best);

            if (attachment != data.nameToLink.end())
            {
                file->Name(best);
                file->Link(attachment->second);

                continue;
            }
        }

        if (data.website == Scraper::Data::Website::AlliedModders)
        {
            // no fallback method when scraping forums
            Fail("Parsing addon files failed.");
            return;
        }

        if (data.url[data.url.size() - 1] != '/')
        {
            file->Link(data.url + '/' + file->Name());
        }
        else
        {
            file->Link(data.url + file->Name());
        }
    }
}

DownloadFiles::DownloadFiles(const LoggerPtr& logger,
                             AddonContext&    context) noexcept
    : Operation(logger, context)
{
}

void DownloadFiles::Run() noexcept
{
    namespace fs = std::filesystem;

    for (const auto& file : GetContext().addon->Files())
    {
        auto path = fs::path{file->Raw()};
        if (!path::CreateIfSafe(path.parent_path()))
        {
            Fail(std::string{"Suspicous file: "} + path.c_str());
            return;
        }

        if (fs::exists(path))
        {
            GetLogger()->Warning() << "Overwriting " << path << cr;
        }
        else
        {
            GetLogger()->Info() << path << cr;
        }

        if (auto error = download::File(file->Link(), path.c_str()))
        {
            Fail(error.message);
            return;
        }
    }
}

MarkInstalled::MarkInstalled(const LoggerPtr& logger,
                             AddonContext&    context) noexcept
    : Operation(logger, context)
{
}

void MarkInstalled::Run() noexcept
{
    GetContext().addon->MarkInstalled();
}
}  // namespace smam
