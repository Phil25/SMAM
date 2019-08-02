#include "addon.h"

#include <fstream>

#include <download.h>

#include <smfs/path.h>

#include <utils/archive.h>
#include <utils/printer.h>

namespace fs = std::filesystem;
using json   = nlohmann::json;

namespace
{
constexpr std::string_view dataFilename    = ".smamdata.json";
constexpr std::string_view dataFilenameBak = ".smamdata.json.bak";

inline void removeFile(const fs::path& file) noexcept
{
    fs::remove(file);
    Path::removeEmpty(file);
}

inline bool prepare(const fs::path& path, const Addon& addon) noexcept
{
    if (Path::prepare(path.parent_path()))
    {
        out(Ch::Warn) << "Ignoring " << path << cr;
        return false;
    }

    bool exists = fs::exists(path);
    out(exists ? Ch::Warn : Ch::Std)
        << (exists ? "Overwriting " : "") << path << cr;

    return true;
}

inline bool fetch(const File::Ptr& file, Addon& addon) noexcept
{
    auto path = fs::path{*file};
    auto url  = file->getUrl();

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

        removeFile(path);
        addon.detach(file);

        return result;
    }

    return true;
}
}  // namespace

Addon::InstalledMap Addon::installed;

Addon::Addon(const std::string& id) : id(id) {}

auto Addon::getId() const noexcept -> std::string { return id; }

auto Addon::getAuthor() const noexcept -> std::string { return author; }

auto Addon::getDescription() const noexcept -> std::string
{
    return description;
}

auto Addon::fileCount() const noexcept -> size_t
{
    return files.size();
}

bool Addon::isExplicit() const noexcept { return installedExplicitly; }

/*
 * Set that the addon has been installed manually by the user
 * (explicit), and not automatically by dependency resolution.
 */
void Addon::markExplicit() noexcept { installedExplicitly = true; }

bool Addon::install(const Scraper::Data& data) noexcept
{
    for (auto& file : files)
    {
        if (!file->evaluate(data) || !fetch(file, *this)) return false;
    }

    addToInstalled();
    return false;
}

void Addon::addToInstalled() noexcept
{
    installed.emplace(id, shared_from_this());
}

bool Addon::isInstalled() const noexcept { return isInstalled(id); }

void Addon::remove() noexcept
{
    remove([](const auto&, const auto&) {});
}

void Addon::remove(const EachFileRemove& cb) noexcept
{
    forEachFile([&cb](auto& file) {
        auto addons = findByFile(file);

        switch (addons.size())  // how many addons own this file
        {
            case 0: cb(file, "nonexistent"); return;
            case 1: break;  // only this one
            default: cb(file, "shared"); return;
        }

        cb(file, "");
        removeFile(*file);
        // no need to detach as whole addon gets purged
    });

    installed.erase(id);
}

void Addon::detach(const File::Ptr& file) noexcept
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

void Addon::forEachFile(const EachFile& cb) noexcept
{
    for (auto& file : files) cb(file);
}

bool Addon::forEachDep(const EachDep& cb) noexcept
{
    for (auto& dep : dependencies)
    {
        if (!cb(dep)) return false;
    }

    return true;
}

auto Addon::get(const std::string& id) noexcept -> AddonOpt
{
    if (isInstalled(id)) return installed[id];
    return std::nullopt;
}

bool Addon::isInstalled(const std::string& id) noexcept
{
    return installed.count(id);
}

void Addon::forEach(const EachAddon& cb) noexcept
{
    for (const auto& [_, addon] : installed) cb(addon);
}

auto Addon::findByFile(const File::Ptr& file) noexcept -> AddonSet
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

void Addon::clear() noexcept { installed.clear(); }

auto Addon::load() noexcept -> LoadResult
{
    if (!Path::gotPermissions(fs::current_path()))
    {
        return LoadResult::NoAccess;
    }

    installed.clear();

    auto p = fs::path{dataFilename};
    if (!fs::exists(p)) return LoadResult::OK;

    auto   ifs    = std::ifstream(p);
    auto   addons = std::vector<std::shared_ptr<Addon>>();
    json   in;
    size_t hash;

    try
    {
        ifs >> in;

        in.at("data").get_to(addons);
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

    for (auto& addon : addons) addon->addToInstalled();

    ifs.close();
    return LoadResult::OK;
}

bool Addon::save() noexcept
{
    std::ofstream ofs(fs::path{dataFilename}, std::ios::trunc);
    if (!ofs) return false;

    std::vector<std::shared_ptr<Addon>> addons;

    for (auto& [_, addon] : installed) addons.push_back(addon);

    json out;
    out["data"] = addons;
    out["hash"] = std::hash<json>{}(out["data"]);

    ofs << out;

    ofs.close();

    return true;
}

void from_json(const json& j, Addon::Ptr& addon)
{
    addon = std::make_shared<Addon>(j.at("id"));

    addon->author              = j.at("author");
    addon->description         = j.at("description");
    addon->installedExplicitly = j.value("explicit", false);

    try  // it's ok for these fields not to exist
    {
        j.at("files").get_to(addon->files);
        j.at("deps").get_to(addon->dependencies);
    }
    catch (const json::exception&)
    {
    }
}

void to_json(json& j, const Addon::Ptr& addon) noexcept
{
    j.emplace("id", addon->id);
    j.emplace("author", addon->author);
    j.emplace("description", addon->description);
    j.emplace("explicit", addon->installedExplicitly);
    j.emplace("files", addon->files);
    j.emplace("deps", addon->dependencies);
}
