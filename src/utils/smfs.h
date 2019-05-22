#pragma once

#include <filesystem>
#include <optional>
#include <set>

#include "file.hpp"

namespace SMFS // SourceMod file system
{
	namespace fs = std::filesystem;
	using MaybePath = std::optional<fs::path>;
	constexpr std::string_view dataFilename = ".smamdata";

	auto findRoot(const fs::path& startAt) -> MaybePath;
	bool isPathSafe(const fs::path&);

	void loadData(const fs::path& filename=dataFilename);
	bool writeData(const fs::path& filename=dataFilename);

	bool regFile(const SMFS::fs::path& file, const std::string& id);
	void removeAddon(const std::string& id, bool print=true);
	void removeEmptyDirs(fs::path startingFrom);

	bool isInstalled(const std::string& id);
	auto getFiles(const std::string& id) -> std::set<fs::path>;
	int countSharedFiles(const fs::path& file);
}
