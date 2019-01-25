#pragma once

#include "database.h"
#include "downloader.h"

namespace Installer{
	// Instantiate all scrapers in static vector of scraper.hpp
	void init(const Database&, Downloader&);

	// Install addon of the specified id
	void install(const std::string& id);
};
