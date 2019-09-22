#pragma once

#include <data/addon.h>
#include <utils/logger.h>

#include <map>
#include <string>

namespace smam
{
class Database final
{
    using CacheMap = std::map<std::string, AddonPlan>;

    /*
     * Maps Addon IDs to {url, addon}
     */
    CacheMap cached;

public:
    Database(Logger&, std::string url,
             const std::vector<std::string>& ids) noexcept;

    auto Cached() noexcept -> const CacheMap&;
};
}  // namespace smam
