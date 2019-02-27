#include <json/json.h>
#include <sstream>

#include "database.h"

static std::string URL = "https://smamdb.net/";

static std::string constructUrl(const std::vector<std::string>& ids)
{
	std::string url(URL + "?ids=");

	for(const auto& id : ids)
	{
		url.append(id + ',');
	}

	url.pop_back(); // remove last comma
	return url;
}

static std::vector<File> toFileVector(Json::Value files)
{
	std::vector<File> vec;

	for(const auto& f : files)
	{
		vec.push_back(File(f.asString()));
	}

	return vec;
}

static Plan makePlan(Json::Value addon)
{
	return Plan(addon["url"].asString(), toFileVector(addon["files"]));
}

Database::Database(Downloader& downloader):
	downloader(downloader)
{
}

void Database::precache(const std::vector<std::string>& ids)
{
	if(ids.empty()) return;

	std::stringstream s(downloader.html(constructUrl(ids)));
	Json::Value root;

	try
	{
		s >> root;

		for(const auto& addon : root)
		{
			precached[addon["id"].asString()] = makePlan(addon);
		}
	}
	catch(const Json::RuntimeError& e)
	{
	}
}

const Plan& Database::get(const std::string& id) const
{
	auto it = precached.find(id);
	return it == precached.end() ? nullPlan : it->second;
}
