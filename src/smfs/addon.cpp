#include "addon.h"

#include <fstream>

#include <download.h>

#include <smfs/path.h>

#include <utils/archive.h>
#include <utils/printer.h>

namespace fs          = std::filesystem;
using json            = nlohmann::json;
using AddonSerializer = nlohmann::adl_serializer<Addon>;

namespace
{
constexpr std::string_view dataFilename    = ".smamdata.json";
constexpr std::string_view dataFilenameBak = ".smamdata.json.bak";

inline void removeFile(const fs::path& file) noexcept
{
    fs::remove(file);
    Utils::Path::removeEmpty(file);
}

inline bool prepare(const fs::path& path, const Addon& addon) noexcept
{
    if (Utils::Path::prepare(path.parent_path()))
    {
        out(Ch::Warn) << "Ignoring " << path << cr;
        return false;
    }

    bool exists = fs::exists(path);
    out(exists ? Ch::Warn : Ch::Std)
        << (exists ? "Overwriting " : "") << path << cr;

    return true;
}

inline bool fetch(const File& file, Addon& addon) noexcept
{
    auto path = fs::path{file};
    auto url  = file.getUrl();

    if (!prepare(path, addon)) return false;

    if (auto error = Download::file(url, path); !error.empty())
    {
        out(Ch::Error) << error << cr;
        return false;
    }

    if (Archive::valid(path))
    {
        out(Ch::Info) << "Extracting " << path.filename() << "..."
                      << cr;

        bool result =
            Archive::extract(path, [&addon](const auto& extractedFile) {
                return prepare(extractedFile, addon);
            });

        fs::remove(path);
        Utils::Path::removeEmpty(path);
        removeFile(path);
        addon.detach(file);

        return result;
    }

    return true;
}
}  // namespace

std::map<std::string, std::shared_ptr<Addon>> Addon::installed;

Addon::Addon(const std::string& id, const std::vector<File>& files,
             const std::string& author, const std::string& description,
             const std::set<std::string>& dependencies)
    : id(id),
      files(files),
      author(author),
      description(description),
      dependencies(dependencies)
{
}

Addon::~Addon() noexcept {}

bool Addon::install(const Scraper::Data& data) noexcept
{
    for (File& file : files)
    {
        file.evaluate(data);
        if (!fetch(file, *this)) return false;
    }

    installed[id] = shared_from_this();  // TODO: emplace
    return false;
}

/*
 * Erase an addon from the local cache.
 */
void Addon::remove() noexcept
{
    remove([](const auto&) {});  // TODO: empty lambda literal?
}

void Addon::remove(const EachFileRemove& cb) noexcept
{
    forEachFile([&cb](const File& file) {
        auto addons = findByFile(file);

        switch (addons.size())  // how many addons own this file
        {
            case 0: cb({file, "nonexistent"}); return;
            case 1: break;  // only this one
            default: cb({file, "shared"}); return;
        }

        cb({file, ""});
        removeFile(file);
    });

    installed.erase(id);
}

void Addon::detach(const File& file) noexcept
{
    for (auto it = files.cbegin(); it != files.cend(); ++it)
    {
        if (*it == file)
        {
            files.erase(it);
            break;
        }
    }
}

/*
 * Set that the addon has been installed manually by the user
 * (explicit), and not automatically by dependency resolution.
 */
void Addon::markExplicit() noexcept { installedExplicitly = true; }

bool Addon::isExplicit() const noexcept { return installedExplicitly; }

bool Addon::isInstalled() const noexcept { return isInstalled(id); }

size_t Addon::getFileCount() const noexcept { return files.size(); }

void Addon::forEachFile(const EachFile& cb) noexcept
{
    for (auto& file : files) cb(file);
}

auto Addon::get(const std::string& id) noexcept -> AddonOpt
{
    if (isInstalled(id)) return installed[id];
    return std::nullopt;
}

/*
 * Return set of files associated with an AddonID.
 */
bool Addon::isInstalled(const std::string& id) noexcept
{
    return installed.count(id);
}

void Addon::forEach(const EachAddon& cb) noexcept
{
    for (const auto& [_, addon] : installed) cb(addon);
}

auto Addon::findByFile(const File& file) noexcept -> AddonSet
{
    AddonSet set;

    forEach([&set, &file](auto addon) {
        const auto& files = addon->files;

        if (std::find(files.begin(), files.end(), file) != files.end())
        {
            set.insert(addon);
        }
    });

    return set;
}

/*
 * Load installed addons from `dataFile` file into `data` map.
 * Return false if not sufficient permissions for either reading or
 * writing.
 */
[[nodiscard]] auto Addon::load() noexcept -> LoadResult
{
    if (!Utils::Path::gotPermissions(fs::current_path()))
    {
        return LoadResult::NoAccess;
    }

    installed.clear();

    auto p = fs::path{dataFilename};
    if (!fs::exists(p)) return LoadResult::OK;

    auto   ifs = std::ifstream(p);
    json   in;
    size_t hash;

    try
    {
        ifs >> in;

        in.at("data").get_to(installed);
        hash = in.at("hash").get<size_t>();
    }
    catch (const json::exception& e)
    {
        return LoadResult::Corrupted;
    }

    if (hash != std::hash<json>{}(in["data"]))
    {
        return LoadResult::Corrupted;
    }

    ifs.close();

    return LoadResult::OK;
}

/*
 * Write loaded data from the `data` map into `dataFile` file.
 */
[[nodiscard]] bool Addon::save() noexcept
{
    std::ofstream ofs(fs::path{dataFilename}, std::ios::trunc);
    if (!ofs) return false;

    json out;
    out["data"] = installed;
    out["hash"] = std::hash<json>{}(out["data"]);

    ofs << out;

    ofs.close();

    return true;
}

auto AddonSerializer::from_json(const json& j) -> Addon
{
    return {j.at("id").get<std::string>(),
            j.at("files").get<std::vector<File>>(),
            j.at("author").get<std::string>(),
            j.at("description").get<std::string>(),
            j.at("deps").get<std::set<std::string>>()};
}

void AddonSerializer::to_json(json& j, const Addon& addon) noexcept
{
    /*j.emplace("id", addon.id);  // TODO change to emplace if ok
    j.emplace("author", addon.author);
    j.emplace("description", addon.description);
    j.emplace("files", addon.files);
    j.emplace("deps", addon.dependencies);*/

    j["id"]          = addon.id;
    j["author"]      = addon.author;
    j["description"] = addon.description;
    j["files"]       = addon.files;
    j["deps"]        = addon.dependencies;
}
