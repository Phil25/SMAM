#pragma once

#include <filesystem>
#include <functional>
#include <optional>
#include <set>

#include "file.hpp"

namespace SMFS // SourceMod file system
{
	namespace fs = std::filesystem;
	using MaybePath = std::optional<fs::path>;
	using NotifyFile = std::function<void(const fs::path&, bool, int)>;
	using ForAddon = std::function<void(const std::string&)>;
	constexpr std::string_view dataFilename = ".smamdata";

	auto findRoot(const fs::path& startAt) -> MaybePath;
	bool isPathSafe(const fs::path&);
	bool prepare(const fs::path&);

	void loadData(const fs::path& filename=dataFilename);
	bool writeData(const fs::path& filename=dataFilename);

	void addFile(const fs::path& file, const std::string& id);
	void removeAddon(const std::string& id, const NotifyFile& cb);
	void removeAddon(const std::string& id);
	void removeEmptyDirs(fs::path startingFrom);

	bool isInstalled(const std::string& id);
	void getInstalled(const ForAddon&);
	auto getFiles(const std::string& id) -> std::set<fs::path>;
	int countSharedFiles(const fs::path& file);
}
