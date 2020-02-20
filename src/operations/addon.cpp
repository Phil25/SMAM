#include "addon.h"

#include <net/download.h>
#include <utils/archive.h>
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

bool ExamineFilePath(const LoggerPtr&             logger,
                     const std::filesystem::path& path) noexcept
{
    if (!path::CreateIfSafe(path.parent_path())) return false;

    if (std::filesystem::exists(path))
    {
        logger->Warning() << "Overwriting " << path << cr;
    }
    else
    {
        logger->Print() << path << cr;
    }

    return true;
}
}  // namespace

AddonContext::AddonContext(const AddonPtr& addon) noexcept
    : addon(addon)
{
}

FindData::FindData(const LoggerPtr& logger, AddonContext& context,
                   const ScraperArrayPtr& scrapers) noexcept
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

    GetContext().data.url = baseUrl;
}

EvaluateFiles::EvaluateFiles(const LoggerPtr& logger,
                             AddonContext&    context) noexcept
    : Operation(logger, context)
{
}

void EvaluateFiles::Run() noexcept
{
    const auto& data = GetContext().data;
    assert(GetContext().addon);

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
    using namespace std::string_literals;

    assert(GetContext().addon);

    for (const auto& file : GetContext().addon->Files())
    {
        auto path = std::filesystem::path{file->Raw()};
        if (!ExamineFilePath(GetLogger(), path))
        {
            Fail("Suspicous file: "s + path.c_str());
            return;
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

    GetLogger()->Info() << "Installed " << Col::green
                        << GetContext().addon->ID() << Col::reset << cr;
}

ExtractArchives::ExtractArchives(const LoggerPtr& logger,
                                 AddonContext&    context) noexcept
    : Operation(logger, context)
{
}

void ExtractArchives::Run() noexcept
{
    using namespace std::string_literals;
    namespace fs = std::filesystem;

    const auto& addon = GetContext().addon;
    assert(addon);

    auto newFiles = FileVector{};

    for (const auto& file : addon->Files())
    {
        auto path = fs::path{file->Raw()};
        assert(fs::exists(path));

        if (!archive::IsValidArchive(path)) continue;

        GetLogger()->Info() << "Extracting " << path.filename() << cr;

        archive::Extract(path, [&](const auto& extracted) {
            if (!ExamineFilePath(GetLogger(), extracted))
            {
                Fail("Suspicous file: "s + path.c_str());
                return;
            }

            newFiles.emplace_back(std::make_shared<File>(extracted));
        });

        fs::remove(path);
        path::RemoveEmptyDirectories(path);
    }

    addon->EraseNonExitentFiles();
    addon->AddFiles(std::move(newFiles));
}

CheckSatisfied::CheckSatisfied(const LoggerPtr& logger,
                               AddonContext&    context) noexcept
    : Operation(logger, context)
{
}

void CheckSatisfied::Run() noexcept
{
    const auto& addon = GetContext().addon;
    assert(addon);

    if (addon->IsInstalled())
    {
        GetLogger()->Info() << "Dependency satisfied: " << Col::green
                            << addon->ID() << Col::reset << cr;
        Stop();
        return;
    }

    GetLogger()->Info() << "Installing dependency: " << Col::green
                        << addon->ID() << Col::reset << "..." << cr;
}
}  // namespace smam
