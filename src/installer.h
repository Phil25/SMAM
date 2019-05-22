#pragma once

#include <string>
#include <vector>
#include <functional>

#include "database.h"
#include "downloader.h"

#include "scrapers/scraper.h"

namespace Installer
{
	using Setup = std::function<bool(const File&)>;
	using FileVector = std::vector<File>;

	void initScrapers(Downloader&);
	auto files(const std::string& id, const Database&) -> FileVector;
	bool setup(const std::string& id, const Database&, Setup);
};
