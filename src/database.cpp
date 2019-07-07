#include "database.h"

#include <nlohmann/json.hpp>

#include "download.h"

using json = nlohmann::json;

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
 * Parse nlohmann::json as a File vector. Implicity converting
 * std::string types.
 */
inline auto toFileVector(const json& files)
{
    return std::vector<File>{files.begin(), files.end()};
}

/*
 * Construct Database::Plan from nlohmann::json
 * Plan = tuple of addon's URL and its File vector (database.h)
 */
inline auto makePlan(const json& addon) -> Database::Plan
{
    return {addon.at("url"), toFileVector(addon.at("files"))};
}
}  // namespace

Database::Plan Database::nullPlan = {"", {}};

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

    auto url = makeUrl(dbUrl, ids);

    try
    {
        json root = json::parse(Download::page(url));

        for (const auto& addon : root)
        {
            cached[addon.at("id")] = makePlan(addon);
        }
    }
    catch (const json::exception& e)
    {
    }
}

/*
 * Return whether an addon was cached. If false, this usually means
 * that it has not been found in the remote database.
 */
bool Database::isPrecached(const std::string& id) const noexcept
{
    return cached.count(id);
}

/*
 * Return Plan of a particiular addon ID. Assuming it's cached.
 * Otherwise, a tuple of empty URL and vector will be returned.
 */
auto Database::get(const std::string& id) const noexcept -> const Plan&
{
    auto it = cached.find(id);
    return it == cached.end() ? nullPlan : it->second;
}
