#pragma once

#include <data/addon.h>

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
    Database(std::string                     url,
             const std::vector<std::string>& ids) noexcept;

    auto Cached() noexcept -> const CacheMap&;
};
}  // namespace smam
