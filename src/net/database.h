#pragma once

#include <data/addon.h>
#include <utils/logger.h>

namespace smam::db
{
auto Fetch(const LoggerPtr&, std::string url,
           const std::vector<std::string>& ids) noexcept -> AddonMapPtr;
}  // namespace smam::db
