#include "installer.h"

#include "scrapers/amscraper.h"


Installer::Installer(const Database& db, Downloader& downloader):
	database(db)
{
	new AMScraper(downloader);
}

void Installer::install(const std::string& id){
	plan.setId(id);
	plan.init(database);
	// TODO: find scraper and fetch
}

const Plan& Installer::getPlan() const{
	return this->plan;
}
