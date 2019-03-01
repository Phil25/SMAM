#pragma once

#include <string>
#include <vector>

#include "database.h"
#include "downloader.h"

#include "scrapers/scraper.h"

namespace Installer
{
	using MaybeScraper = std::optional<std::shared_ptr<Scraper>>;
	using ScraperArray = std::array<std::shared_ptr<Scraper>, 3>;
	using FileVector = std::vector<File>;

	extern ScraperArray scrapers;

	void initScrapers(Downloader&);

	auto getFiles(const std::string& id, const Database&) -> FileVector;

	auto getScraper(const std::string& url) -> MaybeScraper;
};
