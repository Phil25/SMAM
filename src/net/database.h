#pragma once

#include <data/addon.h>
#include <utils/logger.h>

#include <map>
#include <string>

namespace smam
{
class Database final
{
    struct Plan
    {
        std::string url;
        AddonPtr    addon;
    };

    using CacheMap = std::map<std::string, Plan>;

    /*
     * Maps Addon IDs to {url, addon}
     */
    CacheMap cached;

public:
    Database(Logger& logger, std::string url,
             std::initializer_list<std::string> ids) noexcept;

    auto Cached() noexcept -> const CacheMap&;
};
}  // namespace smam
