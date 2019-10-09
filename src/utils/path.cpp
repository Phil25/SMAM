#include "path.h"

namespace smam
{
bool path::IsSafe(const std::filesystem::path& path) noexcept
{
    static const fs::path back = "..";
    constexpr int maxDepth     = 2;  // how far back the path is safe

    int depth = 0;
    for (auto f : path.lexically_normal())
    {
        if (f == back && ++depth > maxDepth) return false;
    }

    return true;
}

auto path::FindSMRoot(const std::filesystem::path& startAt) noexcept
    -> std::optional<std::filesystem::path>
{
    bool exists  = fs::exists(startAt);
    auto current = fs::canonical(exists ? startAt : fs::current_path());
    auto root    = current.root_path();

    fs::path result;
    while (current != root)
    {
        result = current / "addons/sourcemod/";
        if (fs::is_directory(result)) return result;

        current = current.parent_path();
    }

    return std::nullopt;
}

bool path::CreateIfSafe(const fs::path& path) noexcept
{
    if (!IsSafe(path)) return false;

    create_directories(path);
    return true;
}

bool path::HasReadAndWritePermissions(const fs::path& path) noexcept
{
    auto perms = fs::status(path).permissions();
    return (perms & fs::perms::owner_read) != fs::perms::none &&
           (perms & fs::perms::owner_write) != fs::perms::none;
}

void path::RemoveEmptyDirectories(fs::path path) noexcept
{
    while (!path.empty())
    {
        if (fs::is_directory(path) && fs::is_empty(path))
        {
            fs::remove_all(path);
        }

        path = path.parent_path();
    }
}
}  // namespace smam
