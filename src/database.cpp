#include <json/json.h>
#include <sstream>

#include "database.h"

static std::string URL = "https://smamdb.net/";

/*
 * Construct a URL passing requested IDs of addons as an `ids` GET
 * variable, separated by comma.
 *
 * Example: https://smamdb.net/?ids=accelerator,tf2items,thriller
 */
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

/*
 * Parse Json::Value as a File vector.
 */
static std::vector<File> toFileVector(Json::Value files)
{
	std::vector<File> vec;

	for(const auto& f : files)
	{
		vec.push_back(File(f.asString()));
	}

	return vec;
}

/*
 * Construct Plan from Json::Value
 * Plan = tuple of addon's URL and its File vector (database.h)
 */
static Plan makePlan(Json::Value addon)
{
	return Plan(addon["url"].asString(), toFileVector(addon["files"]));
}

Database::Database(Downloader& downloader):
	downloader(downloader)
{
}

/*
 * Precache data of specified addon IDs into a Database
 * held map variable `pracached,` mapping ID strings to Plan.
 *
 * This must be called before actually getting the Plan.
 * The reason being that only the caller knows which addons to anticipate
 * and it's better to fetch them in one larger call than multiple
 * small ones.
 */
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

/*
 * Return Plan of a particiular addon ID. Assuming it's precached.
 * Otherwise, a tuple of empty URL and vector will be returned.
 */
const Plan& Database::get(const std::string& id) const
{
	auto it = precached.find(id);
	return it == precached.end() ? nullPlan : it->second;
}
