#pragma once

#include <filesystem>
#include <optional>

#include "file.hpp"

namespace SMFS // SourceMod file system
{
	namespace fs = std::filesystem;
	using MaybePath = std::optional<fs::path>;

	auto findRoot(const fs::path& startAt) -> MaybePath;
	bool prepare(const fs::path&);
	bool isPathSafe(const fs::path&);
}
