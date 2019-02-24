#include <json/json.h>
#include <iostream>
#include <sstream>

#include "ghscraper.h"

static constexpr std::string_view URL = "https://github.com/";
static constexpr std::string_view URL_API = "https://api.github.com/repos/";
static constexpr std::string_view RELEASES = "releases/latest";

static std::string getRepoUrl(const std::string& url)
{
	std::string repoUrl = url.substr(URL.size());

	if(repoUrl.at(repoUrl.size() -1) != '/')
	{
		repoUrl += '/';
	}

	return repoUrl;
}

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
		std::cerr << "JSON Exception: " << e.what() << std::endl;
	}

	return attachments;
}
