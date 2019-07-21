#pragma once

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include <smfs/addon.h>

class Database final
{
public:
    using Plan    = std::pair<std::string, std::shared_ptr<Addon>>;
    using PlanOpt = std::optional<Plan>;
    using PlanMap = std::map<std::string, Plan>;

private:
    const std::string dbUrl;

    PlanMap cached;

public:
    Database(const std::string& dbUrl) noexcept;

    void precache(const std::vector<std::string>& ids) noexcept;

    auto get(const std::string& id) const noexcept -> PlanOpt;
};
