#pragma once

#include <filesystem>
#include <functional>
#include <optional>
#include <set>

namespace SMFS  // SourceMod file system
{
namespace fs = std::filesystem;

using MaybePath  = std::optional<fs::path>;
using NotifyFile = std::function<void(const fs::path&, bool, int)>;
using EachAddon  = std::function<void(const std::string&)>;
using FindResult = std::pair<std::string, int>;

enum class DeleteResult
{
    OK,
    Shared,
    NotExists,
};

namespace Path
{
bool isSafe(const fs::path&) noexcept;
auto findRoot(const fs::path& startAt) noexcept -> MaybePath;
bool prepare(const fs::path&) noexcept;
bool gotPermissions(const fs::path& path) noexcept;
void removeEmpty(fs::path startingFrom) noexcept;
}  // namespace Path

namespace File
{
void add(const fs::path& file, const std::string& id) noexcept;
auto remove(const fs::path& file) noexcept -> DeleteResult;
auto find(const fs::path& file) noexcept -> FindResult;
}  // namespace File

namespace Addon
{
auto files(const std::string& id) noexcept -> std::set<fs::path>;
void erase(const std::string& id) noexcept;
void author(const std::string& id, const std::string&) noexcept;
void description(const std::string& id, const std::string&) noexcept;
void deps(const std::string& id, const std::set<std::string>&) noexcept;
void markExplicit(const std::string& id) noexcept;
bool isInstalled(const std::string& id) noexcept;
void getInstalled(const EachAddon&) noexcept;
}  // namespace Addon

namespace Data
{
[[nodiscard]] bool load() noexcept;
[[nodiscard]] bool save() noexcept;
}  // namespace Data

}  // namespace SMFS
