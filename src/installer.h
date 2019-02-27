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

	// Initialize scrapers with appropriate Downloader
	void initScrapers(Downloader&);

	// Install addon of the specified id
	auto install(const std::string& id, Database&) -> FileVector;

	// Get std::optional of a scraper
	auto getScraper(const std::string& url) -> MaybeScraper;
};
