#include "installer.h"
#include "plan.h"

#include "scrapers/amscraper.h"

Database database;

void Installer::init(const Database& db, Downloader& downloader){
	database = db;
	new AMScraper(downloader);
}

void Installer::install(const std::string& id){
	Plan plan(id);
	plan.init(database);

	const Scraper* scraper = Scraper::get(plan.getUrl());
	if(!scraper) return;

	plan.fetch(*scraper);
}
