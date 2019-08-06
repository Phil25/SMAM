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

bool prepare(const fs::path& path, const Addon& addon) noexcept
{
    if (!Path::prepare(path.parent_path()))
    {
        out(Ch::Error) << "Dangerous file: " << path << cr;
        return false;
    }

    bool exists = fs::exists(path);
    out(exists ? Ch::Warn : Ch::Std)
        << (exists ? "Overwriting " : "") << path << cr;

    return true;
}

bool fetch(const File::Ptr& file, const Addon& addon) noexcept
{
    auto path = fs::path{*file};
    auto url  = file->getUrl();

    if (!prepare(path, addon)) return false;

    if (auto error = Download::file(url, path); !error.empty())
    {
        out(Ch::Error) << error << cr;
        return false;
    }

    return true;
}

bool extractArchives(Addon& addon) noexcept
{
    auto extracted = std::vector<File::Ptr>();

    bool success = addon.forEachFile([&](const auto& file) {
        auto path = fs::path{*file};
        if (!Archive::valid(path)) return true;

        out(Ch::Info) << "Extracting " << path.filename() << "..."
                      << cr;

        bool res =
            Archive::extract(path, [&](const auto& extractedFile) {
                if (!prepare(extractedFile, addon)) return false;

                extracted.push_back(
                    std::make_shared<File>(extractedFile));

                return true;
            });

        removeFile(path);
        addon.detach(file);

        return res;
    });

    addon.appendFiles(extracted);

    return success;
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

    Download::placeFiles();

    if (!extractArchives(*this)) return false;

    addToInstalled();
    return true;
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
            case 0: cb(file, "nonexistent"); return true;
            case 1: break;  // only this one
            default: cb(file, "shared"); return true;
        }

        cb(file, "");
        removeFile(*file);
        // no need to detach as whole addon gets purged

        return true;
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

void Addon::appendFiles(const std::vector<File::Ptr>& vec) noexcept
{
    files.insert(files.end(), vec.begin(), vec.end());
}

bool Addon::forEachFile(const EachFile& cb) noexcept
{
    for (auto& file : files)
    {
        if (!cb(file)) return false;
    }

    return true;
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

    forEach([&set, &file](const auto& addon) {
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
