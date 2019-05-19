#pragma once

#include <filesystem>
#include <optional>
#include <set>

#include "file.hpp"

namespace SMFS // SourceMod file system
{
	namespace fs = std::filesystem;
	using MaybePath = std::optional<fs::path>;
	using PathVector = std::vector<fs::path>;
	constexpr std::string_view dataFilename = ".smamdata";

	auto findRoot(const fs::path& startAt) -> MaybePath;
	bool prepare(const fs::path&);
	bool isPathSafe(const fs::path&);
	void removeEmptyDirs(fs::path startingFrom);

	void loadData(const fs::path& filename=dataFilename);
	bool writeData(const fs::path& filename=dataFilename);

	void addFile(const std::string& id, const fs::path&);
	void addFiles(const std::string& id, const PathVector&);
	void removeAddon(const std::string& id);

	bool isInstalled(const std::string& id);
	auto getFiles(const std::string& id) -> std::set<fs::path>;
	int countSharedFiles(const fs::path& file);
}
