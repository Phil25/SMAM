#pragma once

#include <variant>

#include <smfs/addon.h>

class Database final
{
public:
    using Plan = std::pair<std::string, AddonPtr>;

private:
    using PlanVar = std::variant<Plan, std::string>;  // potential error

    const std::string dbUrl;

    std::map<std::string, PlanVar> cached;

public:
    Database(const std::string& dbUrl) noexcept;

    bool precache(const std::vector<std::string>& ids) noexcept;

    auto get(const std::string& id) const noexcept -> PlanVar;
};
