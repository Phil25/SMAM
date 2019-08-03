#pragma once

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include <smfs/addon.h>

class Database final
{
    using Plan    = std::pair<std::string, std::shared_ptr<Addon>>;
    using PlanOpt = std::optional<Plan>;

    const std::string           dbUrl;
    std::map<std::string, Plan> cached;

public:
    Database(const std::string& dbUrl) noexcept;

    void precache(const std::vector<std::string>& ids) noexcept;

    auto get(const std::string& id) const noexcept -> PlanOpt;
};
