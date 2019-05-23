#pragma once

#include <filesystem>
#include <functional>

namespace Archive
{
	namespace fs = std::filesystem;
	using FileCb = std::function<bool(const fs::path&)>;

	// Check whether extension is an archive
	bool valid(const fs::path&);

	// Extract archive at `path` in the same directory
	bool extract(const fs::path&, const FileCb& cb);
}
