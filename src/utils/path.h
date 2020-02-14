#pragma once

#include <filesystem>
#include <optional>

namespace smam::path
{
namespace fs = std::filesystem;

bool IsSafe(const fs::path&) noexcept;

auto FindSMRoot(const fs::path& startAt) noexcept
    -> std::optional<fs::path>;

bool CreateIfSafe(const fs::path&) noexcept;

bool HasReadAndWritePermissions(const fs::path&) noexcept;

void RemoveEmptyDirectories(fs::path) noexcept;

auto MD5(const fs::path&) noexcept -> std::string;
}  // namespace smam::path
