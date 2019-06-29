#include "database.h"

#include <json/json.h>
#include <sstream>

#include "download.h"

namespace
{
/*
 * Construct a URL passing requested IDs of addons as an `ids` GET
 * variable, separated by comma.
 *
 * Example: https://smamdb.net/?ids=accelerator,tf2items,thriller
 */
inline auto makeUrl(const std::string&              dbUrl,
                    const std::vector<std::string>& ids) noexcept
{
    std::string url(dbUrl + "?ids=");

    for (const auto& id : ids)
    {
        url.append(id + ',');
    }

    url.pop_back();  // remove last comma
    return url;
}

/*
 * Parse Json::Value as a File vector.
 */
inline auto toFileVector(Json::Value files) noexcept
{
    std::vector<File> vec;

    for (const auto& f : files)
    {
        vec.push_back({f.asString()});
    }

    return vec;
}

/*
 * Construct Plan from Json::Value
 * Plan = tuple of addon's URL and its File vector (database.h)
 */
inline auto makePlan(Json::Value addon) noexcept -> Plan
{
    return Plan(addon["url"].asString(), toFileVector(addon["files"]));
}
}  // namespace

Database::Database(const std::string& dbUrl) noexcept : dbUrl(dbUrl) {}

/*
 * Precache data of specified addon IDs into a Database
 * held map variable `pracached,` mapping ID strings to Plan.
 *
 * This must be called before actually getting the Plan.
 * The reason being that only the caller knows which addons to
 * anticipate and it's better to fetch them in one larger call than
 * multiple small ones.
 */
void Database::precache(const std::vector<std::string>& ids) noexcept
{
    if (ids.empty()) return;

    auto              url = makeUrl(dbUrl, ids);
    std::stringstream s(Download::page(url));
    Json::Value       root;

    try
    {
        s >> root;

        for (const auto& addon : root)
        {
            precached[addon["id"].asString()] = makePlan(addon);
        }
    }
    catch (const Json::RuntimeError& e)
    {
    }
}

/*
 * Return whether an addon was precached. If false, this usually means
 * that it has not been found in the remote database.
 */
bool Database::isPrecached(const std::string& id) const noexcept
{
    return precached.count(id);
}

/*
 * Return Plan of a particiular addon ID. Assuming it's precached.
 * Otherwise, a tuple of empty URL and vector will be returned.
 */
auto Database::get(const std::string& id) const noexcept -> const Plan&
{
    auto it = precached.find(id);
    return it == precached.end() ? nullPlan : it->second;
}
