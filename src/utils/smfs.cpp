#include "smfs.h"

#include <fstream>
#include <map>
#include <set>

namespace
{
// AddonID -> [files]
std::map<std::string, std::set<SMFS::fs::path>> data;

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
 * Return whether owner has read and write permissions in `path`
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
 * Load installed addons from `dataFile` file into `data` map.
 * Return false if not sufficient permissions for either reading or
 * writing.
 */
[[nodiscard]] bool SMFS::Data::load() noexcept
{
    if (!Path::gotPermissions(fs::current_path())) return false;

    std::ifstream ifs(fs::path{dataFilename});
    std::string   id;
    fs::path      file;

    data.clear();

    while (ifs >> id >> file) File::add(file, id);
    ifs.close();

    return true;
}

/*
 * Write loaded data from the `data` map into `dataFile` file
 */
[[nodiscard]] bool SMFS::Data::save() noexcept
{
    std::ofstream ofs(fs::path{dataFilename}, std::ios::trunc);
    if (!ofs) return false;

    for (const auto& [addon, files] : data)
    {
        for (const auto& file : files)
        {
            ofs << addon << ' ' << file << '\n';
        }
    }

    return true;
}

/*
 * Add file to specified addon ID's cache
 */
void SMFS::File::add(const fs::path&    file,
                     const std::string& id) noexcept
{
    if (!fs::is_directory(file)) data[id].insert(file);
}

/*
 * Detach a file from an addon. In other words, remove it from the
 * cached map of loaded addons. Doesn't remove it from disk
 */
bool SMFS::File::detach(const fs::path&    file,
                        const std::string& id) noexcept
{
    if (!Addon::isInstalled(id)) return false;

    data[id].erase(file);
    if (!data[id].size()) Addon::erase(id);

    return true;
}

/*
 * Remove a file related to an addon from the disk
 */
auto SMFS::File::remove(const fs::path& file) noexcept -> DeleteResult
{
    if (!fs::exists(file)) return DeleteResult::NotExists;
    if (File::countShared(file) > 1) return DeleteResult::Shared;

    fs::remove(file);
    Path::removeEmpty(file);
    return DeleteResult::OK;
}

/*
 * Return how many addons share the specified file
 */
int SMFS::File::countShared(const fs::path& file) noexcept
{
    int count = 0;
    for (const auto& addon : data)
    {
        count += addon.second.count(file);
    }
    return count;
}

/*
 * Return set of files associated with an AddonID
 */
auto SMFS::Addon::files(const std::string& id) noexcept
    -> std::set<fs::path>
{
    return isInstalled(id) ? data[id] : std::set<fs::path>{};
}

/*
 * Erase an addon from the local cache
 */
void SMFS::Addon::erase(const std::string& id) noexcept
{
    data.erase(id);
}

/*
 * Return whether an addon of the given ID is installed
 */
bool SMFS::Addon::isInstalled(const std::string& id) noexcept
{
    return data.count(id);
}

/*
 * Iterate over installed addons
 */
void SMFS::Addon::getInstalled(const EachAddon& cb) noexcept
{
    for (const auto& addon : data) cb(addon.first);
}
