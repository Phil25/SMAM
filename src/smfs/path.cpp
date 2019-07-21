#include "path.h"

/*
 * Make sure the path doesn't escape up to 2 directories
 * before the current path.
 */
bool Utils::Path::isSafe(const fs::path& path) noexcept
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
auto Utils::Path::findRoot(const fs::path& st) noexcept -> MaybePath
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
bool Utils::Path::prepare(const fs::path& path) noexcept
{
    if (!Path::isSafe(path)) return false;
    create_directories(path);
    return true;
}

/*
 * Return whether owner has read and write permissions in `path`.
 */
bool Utils::Path::gotPermissions(const fs::path& path) noexcept
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
void Utils::Path::removeEmpty(fs::path p) noexcept
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
