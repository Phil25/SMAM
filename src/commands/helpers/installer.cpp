#include "installer.h"

#include <regex>

#include <download.h>
#include <smfs.h>

#include <scrapers/amscraper.h>
#include <scrapers/ghscraper.h>
#include <scrapers/ltscraper.h>
#include <scrapers/scraper.h>

#include <utils/archive.h>
#include <utils/misc.h>
#include <utils/printer.h>
#include <utils/version.h>

namespace fs       = std::filesystem;
using StringVector = std::vector<std::string>;
using Type         = Report::Type;

namespace
{
using DependencyRemark =
    std::function<std::string(const std::string&, const std::string&)>;

const std::map<Type, DependencyRemark> remarkMap{
    {Type::Installed,
     [](const std::string& a, const std::string& d) {
         return "Installed " + d + " as dependency of " + a + '.' + cr;
     }},

    {Type::Skipped,
     [](const std::string& a, const std::string& d) {
         return "Dependency " + d + " of " + a + " already satisfied." +
                cr;
     }},

    {Type::Failed,
     [](const std::string& a, const std::string& d) {
         return "Dependency " + d + " of " + a + " failed to install." +
                cr;
     }},
};

inline auto wrap(const std::string& addon) noexcept
{
    return out.parse(Col::green) + addon + out.parse(Col::reset);
}

inline auto getRemark(Type type, const std::string& addon,
                      const std::string& dependency) noexcept
{
    auto a = wrap(addon);
    auto d = wrap(dependency);

    return remarkMap.count(type) ? remarkMap.at(type)(a, d)
                                 : std::string();
}

/*
 * Return vector of Attachment names matched against `base` regex.
 */
inline auto findMatches(const std::string&   base,
                        const Scraper::Data& data) noexcept
    -> StringVector
{
    StringVector filtered;

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
        out(Ch::Error) << "Invalid regex: \"" << base << '\"' << cr;
    }

    return filtered;
}

/*
 * Process File vector altering its instances based on addon's URL
 * and Attachments (map of file names from addon's URL to their
 * individual download URLs; ./scrapers/scraper.h).
 * Attachments are received from a Scraper and can be an empty
 * object.
 *
 * The purpose is to obtain FileVector, each instance having
 * appropriate File::name (instead of ex. a link or regex string),
 * and File::url (URL from which the file can be downloaded).
 *
 * In most situations only File::url gets altered. In case of
 * File::name's containing a link or regex, both fields are altered.
 *
 * This function will do the following for each File instance:
 *
 * - Check if File also appears in Attachments.
 *   If so, it is assumed that the name is correct. URL is set.
 *
 * - Check if File::name is a link.
 *   If so, it is assumed that it ends in the actual file name.
 *   Name and URL are set.
 *
 * - Check if matches in Attachments can be found when File
 *   name is treated as a regex string.
 *   If multiple matches are found, it is assumed that Attachments
 *   consist of similar files with a version number in the name of
 *   each. (file-2.3.txt, file-2.4.txt, file-2.4.1.txt, etc...) File
 *   name is set to the one containing the largest version number.
 *   File URL is set to the Attachment URL.
 *
 * - Otherwise (the general case) it is assumed that the File
 *   download URL can be obtained by combining the addon's base URL
 *   and the File name. URL is set.
 */
inline void processFiles(std::vector<File>&   files,
                         const Scraper::Data& data) noexcept
{
    for (File& file : files)
    {
        auto attachment = data.find(file.name);
        if (attachment != data.end())  // found
        {
            file.url = attachment->second;
            continue;
        }

        if (Utils::isLink(file.name))  // specified file is a link
        {
            size_t pos = file.name.rfind('/');
            if (pos != std::string::npos)
            {
                file.url  = file.name;
                file.name = file.name.substr(++pos);
            }

            continue;
        }

        auto names = findMatches(file.name, data);

        if (!names.empty())
        {
            std::string name = Utils::Version::biggest(names);
            attachment       = data.find(name);

            if (attachment != data.end())
            {
                file.name = name;
                file.url  = attachment->second;
                continue;
            }
        }

        if (data.website == Scraper::Data::Website::AlliedModders)
        {
            file.invalidate();
        }
        else
        {
            file.url = data.url + file.name;
        }
    }
}

bool registerFile(const fs::path&    file,
                  const std::string& addon) noexcept
{
    if (!SMFS::Path::prepare(file.parent_path()))
    {
        out(Ch::Warn) << "Ignoring " << file << cr;
        return false;
    }

    bool exists = fs::exists(file);
    out(exists ? Ch::Warn : Ch::Std)
        << (exists ? "Overwriting " : "") << file << cr;

    SMFS::File::add(file, addon);
    return true;
}

bool installFile(const File& data, const std::string& id) noexcept
{
    if (!data.valid())
    {
        out(Ch::Error) << "Invalid file: " << data.name << cr;
        return false;
    }

    fs::path file = data.path;
    file /= data.name;

    if (!registerFile(file, id)) return false;

    if (auto error = Download::file(data.url, file); !error.empty())
    {
        out(Ch::Error) << error << cr;
        return false;
    }

    if (Archive::valid(file))
    {
        out(Ch::Info) << "Extracting " << data.name << "..." << cr;

        bool result = Archive::extract(
            file, [&id](const fs::path& extractedFile) {
                return registerFile(extractedFile, id);
            });

        return result &&
               SMFS::File::remove(file) == SMFS::DeleteResult::OK;
    }

    return true;
}
}  // namespace

Installer::Installer(const std::string&  databaseUrl,
                     const StringVector& addons, bool forceInstall,
                     bool noDeps) noexcept
    : database(databaseUrl),
      addons(addons),
      forceInstall(forceInstall),
      noDeps(noDeps),
      failedCount(0)
{
    Scraper::make(0, std::make_shared<AMScraper>());
    Scraper::make(1, std::make_shared<LTScraper>());
    Scraper::make(2, std::make_shared<GHScraper>());

    database.precache(addons);
}

auto Installer::installAll() noexcept -> const Report&
{
    for (const auto& addon : addons)
    {
        report.insert(installSingle(addon), addon);
        SMFS::Addon::markExplicit(addon);
    }

    return report;
}

/*
 * Install a single specific addon that has been precached by the
 * database.
 */
auto Installer::installSingle(const std::string& addon) noexcept -> Type
{
    if (pending.count(addon))  // already being installed
    {
        return Type::Queued;
    }

    if (SMFS::Addon::isInstalled(addon) && !forceInstall)
    {
        out(Ch::Info) << "Already installed: " << addon << cr << cr;
        return Type::Skipped;
    }

    if (!database.isPrecached(addon))
    {
        out(Ch::Error) << Col::red << "Not found: " << addon
                       << Col::reset << cr << cr;
        return Type::Failed;
    }

    pending.insert(addon);  // circumvent cyclic deps inf loop
    auto data    = get(addon);
    bool success = true;

    for (const auto& dep : data.dependencies)
    {
        if (noDeps)
        {
            if (!SMFS::Addon::isInstalled(dep))
            {
                report.remark("Dependency " + wrap(dep) + " of " +
                              wrap(addon) + " was not installed.");
            }
        }
        else
        {
            auto depResult = installSingle(dep);
            report.remark(getRemark(depResult, addon, dep));

            if (depResult == Type::Failed)
            {
                success = false;
                break;
            }
        }
    }

    out(Ch::Info) << Col::green << "Installing " << addon << "..."
                  << Col::reset << cr;

    for (const auto& file : data.files)
    {
        if (!installFile(file, addon))
        {
            success = false;
            break;
        }
    }

    if (!success)
    {
        out(Ch::Error) << Col::red << "Failed to install " << addon
                       << Col::reset << cr << cr;

        for (const auto& file : SMFS::Addon::files(addon))
        {
            SMFS::File::remove(file);
        }

        SMFS::Addon::erase(addon);
        return Type::Failed;
    }

    SMFS::Addon::author(addon, data.author);
    SMFS::Addon::description(addon, data.description);
    SMFS::Addon::deps(addon, data.dependencies);

    out << cr;
    return Type::Installed;
}

auto Installer::get(const std::string& id) noexcept -> Database::Addon
{
    auto [url, addonData] = database.get(id);

    if (!url.empty())
    {
        Scraper::Data scraperData;

        if (auto scraper = Scraper::get(url))
        {
            scraperData = scraper->get()->fetch(url);
        }

        scraperData.url = url;
        processFiles(addonData.files, scraperData);
    }

    return addonData;
}
