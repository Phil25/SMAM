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
Database::Database(Logger& logger, std::string url,
                   const std::vector<std::string>& ids) noexcept
{
    using json = nlohmann::json;

    url = BuildQueryURL(std::move(url), ids);

    auto root = json::parse(download::Html(url), nullptr, false);
    if (root.is_discarded())
    {
        logger.Error() << "Query result parsing failed.";
        return;
    }

    for (const auto& entry : root)
    {
        auto id = entry.value("id", "");
        if (id.empty()) continue;

        try
        {
            auto addon = entry.get<AddonPtr>();
            addon->BaseURL(entry.at("url"));

            cached.emplace(std::move(id), addon);
        }
        catch (const json::exception& e)
        {
            logger.Error() << e.what() << cr;
        }
        catch (const std::invalid_argument& e)
        {
            logger.Error() << e.what() << cr;
        }
    }
}

auto Database::Cached() noexcept -> const AddonMap&
{
    return cached;
}
}  // namespace smam
