#include <json/json.h>
#include <sstream>

#include "ghscraper.h"

#include "../utils/printer.h"

static constexpr std::string_view URL = "https://github.com/";
static constexpr std::string_view URL_API = "https://api.github.com/repos/";
static constexpr std::string_view RELEASES = "releases/latest";

/*
 * Get GitHub repository address from a full link.
 * https://github.com/user/repo/ -> user/repo/
 */
static std::string getRepoUrl(const std::string& url)
{
	std::string repoUrl = url.substr(URL.size());

	if(repoUrl.at(repoUrl.size() -1) != '/')
	{
		repoUrl += '/';
	}

	return repoUrl;
}

/*
 * Get API URL to the latest releases from a particular GitHub
 * repository address.
 * user/repo -> https://api.github.com/repos/user/repo/releases/latest
 */
static std::string getReleasesUrl(const std::string& repoUrl)
{
	return std::string(URL_API).append(repoUrl).append(RELEASES);
}

GHScraper::GHScraper(Downloader& downloader):
	Scraper(downloader, URL)
{
}

GHScraper::~GHScraper()
{
}

Attachments GHScraper::fetch(const std::string& url)
{
	std::stringstream s(downloader.html(getReleasesUrl(getRepoUrl(url))));
	Json::Value root;
	Attachments attachments{};

	try
	{
		s >> root;
		std::string name;
		std::string url;

		for(const auto& asset : root["assets"])
		{
			name = asset["name"].asString();
			url = asset["browser_download_url"].asString();
			attachments[name] = url;
		}
	}
	catch(const Json::RuntimeError& e)
	{
		out(Ch::Error) << e.what() << cr;
	}

	return attachments;
}
