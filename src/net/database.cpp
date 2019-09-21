#include "database.h"

#include <net/download.h>

#include <nlohmann/json.hpp>

namespace
{
auto BuildQueryURL(std::string                     url,
                   const std::vector<std::string>& ids) noexcept
    -> std::string
{
    assert(ids.size());

    url.append("?ids=");

    for (const auto& id : ids) url.append(id + ',');

    url.pop_back();  // remove last comma
    return url;
}
}  // namespace

namespace smam
{
Database::Database(std::string                     url,
                   const std::vector<std::string>& ids) noexcept
{
    using json = nlohmann::json;

    url = BuildQueryURL(std::move(url), ids);

    auto root = json::parse(download::Html(url), nullptr, false);
    if (root.is_discarded())
    {
        return;
    }

    for (const auto& entry : root)
    {
        auto id = entry.value("id", "");
        if (id.empty()) continue;

        try
        {
            auto plan = Plan{entry.at("url"), entry.get<AddonPtr>()};
            cached.emplace(std::move(id), std::move(plan));
        }
        catch (const json::exception& e)
        {
        }
        catch (const std::invalid_argument& e)
        {
        }
    }
}

auto Database::Cached() noexcept -> const CacheMap&
{
    return cached;
}
}  // namespace smam
