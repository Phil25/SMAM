#pragma once

#include <filesystem>
#include <functional>

namespace Archive
{
	namespace fs = std::filesystem;
	using PathVector = std::vector<fs::path>;
	using FileCb = std::function<bool(const fs::path&)>;

	// Check whether extension is an archive
	bool valid(const fs::path&);

	// Extract archive at `path` in the same directory
	auto extract(const fs::path&, FileCb cb) -> PathVector;
}
