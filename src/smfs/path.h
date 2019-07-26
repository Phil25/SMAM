#pragma once

#include <filesystem>
#include <optional>

namespace Path
{
namespace fs    = std::filesystem;
using MaybePath = std::optional<fs::path>;

bool isSafe(const fs::path&) noexcept;
auto findRoot(const fs::path& startAt) noexcept -> MaybePath;
bool prepare(const fs::path&) noexcept;
bool gotPermissions(const fs::path& path) noexcept;
void removeEmpty(fs::path startingFrom) noexcept;
}  // namespace Path
