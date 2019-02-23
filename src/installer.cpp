#include <iostream>
#include <regex>

#include "installer.h"
#include "utils/file.hpp"
#include "utils/version.h"

#include "scrapers/amscraper.h"
#include "scrapers/ltscraper.h"

static std::vector<std::string> findMatches(
	const std::string base,
	const Attachments& attachments
){
	std::vector<std::string> filtered;

	try
	{
		const std::regex re(base);

		for(const auto& [name, url] : attachments)
		{
			if(std::regex_match(name, re))
			{
				filtered.push_back(name);
			}
		}
	}
	catch(const std::regex_error& e)
	{
		std::cerr << "Invalid regex: " << base << "\" " << std::endl;
	}

	return filtered;
}

static void parse(
	const std::string& url,
	std::vector<File>& files,
	const Attachments& attachments // map<string, string>
){
	for(File& file : files)
	{
		auto attachment = attachments.find(file.name);
		if(attachment != attachments.end()) // found
		{
			file.url = attachment->second;
			continue;
		}

		if(Utils::isLink(file.name)) // specified file is a link
		{
			size_t pos = file.name.rfind('/');
			if(pos != std::string::npos)
			{
				file.url = file.name;
				file.name = file.name.substr(++pos);
			}

			continue;
		}

		auto names = findMatches(file.name, attachments);
		if(names.size())
		{
			std::string name = Utils::Version::biggest(names);
			attachment = attachments.find(name);

			if(attachment != attachments.end())
			{
				file.name = name;
				file.url = attachment->second;
				continue;
			}
		}

		file.url = url + file.name;
	}
}

Installer::ScraperArray Installer::scrapers;

void Installer::initScrapers(Downloader& d)
{
	scrapers[0] = std::make_shared<AMScraper>(d);
	scrapers[1] = std::make_shared<LTScraper>(d);
}

auto Installer::install(const std::string& id, Database& db) -> FileVector
{
	auto [url, files] = db.fetch(id);

	if(url.size())
	{
		Attachments attachments;

		if(MaybeScraper scraper = getScraper(url))
		{
			attachments = scraper->get()->fetch(url);
		}

		parse(url, files, attachments);
	}

	return files;
}

void Installer::install(const std::vector<std::string>& ids, Database& db)
{
	for(const std::string& id : ids)
	{
		if(!install(id, db).size())
		{
			std::cerr << "Could not find addon: " << id << std::endl;
		}
	}
}

auto Installer::getScraper(const std::string& url) -> MaybeScraper
{
	for(const auto& scraper : scrapers)
	{
		if(scraper->match(url))
		{
			return scraper;
		}
	}

	return std::nullopt;
}
