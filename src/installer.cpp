#include <regex>

#include "installer.h"
#include "utils/file.hpp"
#include "utils/printer.h"
#include "utils/version.h"

#include "scrapers/amscraper.h"
#include "scrapers/ltscraper.h"
#include "scrapers/ghscraper.h"

/*
 * Return vector of Attachment names matched against `base` regex.
 */
static std::vector<std::string> findMatches(
	const std::string& base,
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
		out(Ch::Error) << "Invalid regex: " << base << '\"' << cr;
	}

	return filtered;
}

/*
 * Process File vector altering its instances based on addon's URL
 * and Attachments (map of file names from addon's URL to their
 * individual download URLs; ./scrapers/scraper.h).
 * Attachments are received from a Scraper and can be an empty object.
 *
 * The purpose is to obtain a File vector, each instance having appropriate
 * File::name (instead of ex. a link or regex string), and
 * File::url (URL from which the file can be downloaded).
 *
 * In most situations only File::url gets altered. In case of
 * File::name's containing a link or regex, both fields are altered.
 *
 * This function will do the following for each File instance:
 *
 * - Check if File also appears in Attachments.
 *   If so, it is assumed that the name is correct. URL is set.
 *
 * - Check if File::name is a link.
 *   If so, it is assumed that it ends in the actual file name.
 *   Name and URL are set.
 *
 * - Check if matches in Attachments can be found when File
 *   name is treated as a regex string.
 *   If multiple matches are found, it is assumed that Attachments
 *   consist of similar files with a version number in the name of each.
 *   (file-2.3.txt, file-2.4.txt, file-2.4.1.txt, etc...)
 *   File name is set to the one containing the largest version number.
 *   File URL is set to the Attachment URL.
 *
 * - Otherwise (the general case) it is assumed that the File download
 *   URL can be obtained by combining the addon's base URL and the File
 *   name. URL is set.
 */
static void processFiles(
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
		if(!names.empty())
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

/*
 * Initialize std::array<Scraper, 3> of usable Scraper instances.
 */
void Installer::initScrapers(Downloader& d)
{
	scrapers[0] = std::make_shared<AMScraper>(d);
	scrapers[1] = std::make_shared<LTScraper>(d);
	scrapers[2] = std::make_shared<GHScraper>(d);
}

/*
 * Return vector of fully processed File instances.
 */
auto Installer::getFiles(Utils::cstr& id, const Database& db) -> FileVector
{
	auto [url, files] = db.get(id);

	if(!url.empty())
	{
		Attachments attachments;

		if(MaybeScraper scraper = getScraper(url))
		{
			attachments = scraper->get()->fetch(url);
		}

		processFiles(url, files, attachments);
	}

	return files;
}

/*
 * Return appropriate Scraper for given URL or null.
 */
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
