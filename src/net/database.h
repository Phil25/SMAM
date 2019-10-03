#pragma once

#include <data/addon.h>
#include <utils/logger.h>

#include <map>
#include <string>

namespace smam
{
class Database final
{
    /*
     * Maps Addon IDs to AddonPtr
     */
    AddonMap cached;

public:
    Database(Logger&, std::string url,
             const std::vector<std::string>& ids) noexcept;

    auto Cached() noexcept -> const AddonMap&;
};
}  // namespace smam
