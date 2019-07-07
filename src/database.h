#pragma once

#include <map>
#include <string>
#include <tuple>
#include <vector>

#include "utils/file.hpp"

// Addon's URL & vector of assoiated Files.
using Plan    = std::tuple<std::string, std::vector<File>>;
using PlanMap = std::map<std::string, Plan>;

class Database final
{
    const std::string dbUrl;

    Plan    nullPlan = {"", {}};
    PlanMap cached;

public:
    Database(const std::string& dbUrl) noexcept;

    void precache(const std::vector<std::string>& ids) noexcept;
    bool isPrecached(const std::string& id) const noexcept;

    const Plan& get(const std::string& id) const noexcept;
};
