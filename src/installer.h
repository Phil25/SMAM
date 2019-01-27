#pragma once

#include "plan.h"
#include "database.h"
#include "downloader.h"

class Installer{
	const Database database;
	Plan plan;

public:
	// Instantiate all scrapers in static vector of scraper.hpp
	Installer(const Database&, Downloader&);

	// Install addon of the specified id
	void install(const std::string& id);

	// get the current plan of the installation
	const Plan& getPlan() const;
};
