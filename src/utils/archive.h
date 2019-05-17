#pragma once

#include <filesystem>

namespace Archive
{
	namespace fs = std::filesystem;

	// Check whether extension is an archive
	bool valid(const fs::path&);

	// Extract archive at `path` in the same directory
	auto extract(const fs::path&) -> std::vector<fs::path>;
}
