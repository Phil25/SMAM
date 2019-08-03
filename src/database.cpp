#include "database.h"

#include <download.h>

#include <nlohmann/json.hpp>

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
}  // namespace

Database::Database(const std::string& dbUrl) noexcept : dbUrl(dbUrl) {}

/*
 * Cache data of specified addon IDs into a Database held map variable
 * `cached,` mapping ID strings to PlanRef.
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

        for (const auto& j : root)
        {
            auto addon = j.get<std::shared_ptr<Addon>>();
            cached.emplace(j.at("id"), Plan{j.at("url"), addon});
        }
    }
    catch (const json::exception& e)
    {
    }
}

/*
 * Return std::optional<PlanRef> of a particiular addon ID. Assuming
 * it's cached. Otherwise, a nullopt is returned.
 */
auto Database::get(const std::string& id) const noexcept -> PlanOpt
{
    if (auto it = cached.find(id); it != cached.end())
    {
        return it->second;
    }

    return std::nullopt;
}
