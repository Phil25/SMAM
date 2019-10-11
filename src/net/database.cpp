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
auto db::Fetch(const LoggerPtr& logger, std::string url,
               const std::vector<std::string>& ids) noexcept
    -> AddonMapPtr
{
    using json = nlohmann::json;

    url = BuildQueryURL(std::move(url), ids);

    auto root = json::parse(download::Html(url), nullptr, false);
    if (root.is_discarded())
    {
        logger->Error() << "Query result parsing failed.";
        return {};
    }

    auto cache = std::make_shared<AddonMap>();

    for (const auto& entry : root)
    {
        auto id = entry.value("id", "");
        if (id.empty()) continue;

        try
        {
            auto addon = entry.get<AddonPtr>();
            addon->BaseURL(entry.at("url"));

            cache->emplace(std::move(id), addon);
        }
        catch (const json::exception& e)
        {
            logger->Error() << e.what() << cr;
        }
        catch (const std::invalid_argument& e)
        {
            logger->Error() << e.what() << cr;
        }
    }

    return cache;
}
}  // namespace smam
