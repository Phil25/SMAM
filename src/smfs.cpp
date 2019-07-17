#include "smfs.h"

#include <fstream>
#include <map>
#include <set>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace
{
struct Addon final
{
    std::string              author, description;
    bool                     installedExplicitly;
    std::set<SMFS::fs::path> files;
    std::set<std::string>    deps;
};

void to_json(json& j, const Addon& a) noexcept
{
    j["author"]      = a.author;
    j["description"] = a.description;
    j["explicit"]    = a.installedExplicitly;
    j["files"]       = a.files;
    j["deps"]        = a.deps;
}

void from_json(const json& j, Addon& a) noexcept
{
    a.author              = j["author"];
    a.description         = j["description"];
    a.installedExplicitly = j["explicit"];

    if (j.count("files")) j["files"].get_to(a.files);
    if (j.count("deps")) j["deps"].get_to(a.files);
}

std::map<std::string, Addon> data;

constexpr std::string_view dataFilename = ".smamdata";
}  // namespace

/*
 * Make sure the path doesn't escape up to 2 directories
 * before the current path.
 */
bool SMFS::Path::isSafe(const fs::path& path) noexcept
{
    static const fs::path back = "..";

    int depth = 0;
    for (auto f : path.lexically_normal())
    {
        if (f == back && ++depth > 2) return false;
    }

    return true;
}

/*
 * Finds SourceMod root ([...]/mod/addons/sourcemod)
 * from the ./mod directory or above.
 */
auto SMFS::Path::findRoot(const fs::path& st) noexcept -> MaybePath
{
    auto cur  = fs::canonical(fs::exists(st) ? st : fs::current_path()),
         root = cur.root_path();

    fs::path res;
    while (cur != root)
    {
        res = cur / "addons/sourcemod/";
        if (fs::is_directory(res)) return res;

        cur = cur.parent_path();
    }

    return std::nullopt;
}

/*
 * Safely prepare directories for a file of an addon, preveting
 * the file.path to go beyond the allowed directory structure.
 */
bool SMFS::Path::prepare(const fs::path& path) noexcept
{
    if (!Path::isSafe(path)) return false;
    create_directories(path);
    return true;
}

/*
 * Return whether owner has read and write permissions in `path`.
 */
bool SMFS::Path::gotPermissions(const fs::path& path) noexcept
{
    auto perms = fs::status(path).permissions();
    return (perms & fs::perms::owner_read) != fs::perms::none &&
           (perms & fs::perms::owner_write) != fs::perms::none;
}

/*
 * Remove empty directories starting from the specified path,
 * iterating into shallower directories. This path should always
 * be (in practice) relative to the SourceMod root directory.
 */
void SMFS::Path::removeEmpty(fs::path p) noexcept
{
    while (!p.empty())
    {
        if (fs::is_directory(p) && fs::is_empty(p))
        {
            fs::remove_all(p);
        }

        p = p.parent_path();
    }
}

/*
 * Add file to specified addon ID's cache.
 */
void SMFS::File::add(const fs::path&    file,
                     const std::string& id) noexcept
{
    if (!fs::is_directory(file)) data[id].files.insert(file);
}

/*
 * Remove a file related to an addon from the disk.
 */
auto SMFS::File::remove(const fs::path& file) noexcept -> DeleteResult
{
    const auto& [id, count] = File::find(file);

    switch (count)
    {
        case 0: return DeleteResult::NotExists;
        case 1: break;
        default: return DeleteResult::Shared;
    }

    fs::remove(file);
    Path::removeEmpty(file);
    data[id].files.erase(file);

    return DeleteResult::OK;
}

/*
 * Find which and how many addons owns the specified file if any. If
 * multiple addons own the file, return the ID of the last one found
 * (usually is one).
 */
auto SMFS::File::find(const fs::path& file) noexcept -> FindResult
{
    std::string lastId;
    int         count = 0;

    for (const auto& [id, addon] : data)
    {
        if (addon.files.count(file))
        {
            ++count;
            lastId = id;
        }
    }

    return {lastId, count};
}

/*
 * Return set of files associated with an AddonID.
 */
auto SMFS::Addon::files(const std::string& id) noexcept
    -> std::set<fs::path>
{
    return isInstalled(id) ? data[id].files : std::set<fs::path>{};
}

/*
 * Erase an addon from the local cache.
 */
void SMFS::Addon::erase(const std::string& id) noexcept
{
    data.erase(id);
}

/*
 * Set whether the addon has been installed automatically by dependency
 * resolution (non explicit), or manually by the user (explicit).
 */
void SMFS::Addon::markExplicit(const std::string& id) noexcept
{
    if (isInstalled(id)) data[id].installedExplicitly = true;
}

/*
 * Set the author field of the addon.
 */
void SMFS::Addon::author(const std::string& id,
                         const std::string& author) noexcept
{
    if (isInstalled(id)) data[id].author = author;
}

/*
 * Set the author field of the addon.
 */
void SMFS::Addon::description(const std::string& id,
                              const std::string& description) noexcept
{
    if (isInstalled(id)) data[id].description = description;
}

/*
 * Add addon dependency.
 */
void SMFS::Addon::deps(const std::string&           id,
                       const std::set<std::string>& deps) noexcept
{
    if (isInstalled(id)) data[id].deps = deps;
}

/*
 * Return whether an addon of the given ID is installed.
 */
bool SMFS::Addon::isInstalled(const std::string& id) noexcept
{
    return data.count(id);
}

/*
 * Iterate over installed addons.
 */
void SMFS::Addon::getInstalled(const EachAddon& cb) noexcept
{
    for (const auto& [id, _] : data) cb(id);
}

/*
 * Load installed addons from `dataFile` file into `data` map.
 * Return false if not sufficient permissions for either reading or
 * writing.
 */
[[nodiscard]] bool SMFS::Data::load() noexcept
{
    if (!Path::gotPermissions(fs::current_path())) return false;

    data.clear();

    std::ifstream ifs(fs::path{dataFilename});
    json          in;

    try
    {
        ifs >> in;
        in["data"].get_to(data);
    }
    catch (const json::exception& e)
    {
    }

    ifs.close();

    return true;
}

/*
 * Write loaded data from the `data` map into `dataFile` file.
 */
[[nodiscard]] bool SMFS::Data::save() noexcept
{
    std::ofstream ofs(fs::path{dataFilename}, std::ios::trunc);
    if (!ofs) return false;

    json out;
    out["data"] = data;
    out["hash"] = std::hash<json>{}(out["data"]);

    ofs << out;

    ofs.close();

    return true;
}
