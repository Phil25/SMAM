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
using ForAddon   = std::function<void(const std::string&)>;

enum class DeleteResult
{
    OK,
    NotExists,
    Shared
};

auto findRoot(const fs::path& startAt) noexcept -> MaybePath;
bool isPathSafe(const fs::path&) noexcept;
bool prepare(const fs::path&) noexcept;
bool gotPermissions(const fs::path& path) noexcept;

[[nodiscard]] bool loadData() noexcept;
[[nodiscard]] bool writeData() noexcept;

void addFile(const fs::path& file, const std::string& id) noexcept;
bool eraseFile(const fs::path& file, const std::string& id) noexcept;
void eraseAddon(const std::string& id) noexcept;

auto removeFile(const fs::path& file) noexcept -> DeleteResult;
void removeEmptyDirs(fs::path startingFrom) noexcept;

bool isInstalled(const std::string& id) noexcept;
void getInstalled(const ForAddon&) noexcept;
auto getFiles(const std::string& id) noexcept -> std::set<fs::path>;
int  countSharedFiles(const fs::path& file) noexcept;
}  // namespace SMFS
