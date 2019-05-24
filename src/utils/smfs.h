#pragma once

#include <filesystem>
#include <functional>
#include <optional>
#include <set>

#include "file.hpp"

namespace SMFS  // SourceMod file system
{
namespace fs     = std::filesystem;
using MaybePath  = std::optional<fs::path>;
using NotifyFile = std::function<void(const fs::path&, bool, int)>;
using ForAddon   = std::function<void(const std::string&)>;
constexpr std::string_view dataFilename = ".smamdata";

enum class DeleteResult
{
    OK,
    NotExists,
    Shared
};

auto findRoot(const fs::path& startAt) noexcept -> MaybePath;
bool isPathSafe(const fs::path&) noexcept;
bool prepare(const fs::path&) noexcept;

void loadData(const fs::path& filename = dataFilename) noexcept;
bool writeData(const fs::path& filename = dataFilename) noexcept;

void addFile(const fs::path& file, const std::string& id) noexcept;
bool eraseFile(const fs::path& file, const std::string& id) noexcept;

auto removeFile(const fs::path& file) noexcept -> DeleteResult;
void removeAddon(const std::string& id) noexcept;
void removeEmptyDirs(fs::path startingFrom) noexcept;

bool isInstalled(const std::string& id) noexcept;
void getInstalled(const ForAddon&) noexcept;
auto getFiles(const std::string& id) noexcept -> std::set<fs::path>;
int  countSharedFiles(const fs::path& file) noexcept;
}  // namespace SMFS
