#pragma once

#include <map>
#include <string>
#include <vector>

#include <utils/file.hpp>

class Database final
{
public:
    struct Addon final
    {
        std::string              author, description;
        std::vector<File>        files;
        std::vector<std::string> dependencies;
    };

    using Plan    = std::pair<std::string, Addon>;
    using PlanMap = std::map<std::string, Plan>;

private:
    static Plan nullPlan;

    const std::string dbUrl;

    PlanMap cached;

public:
    Database(const std::string& dbUrl) noexcept;

    void precache(const std::vector<std::string>& ids) noexcept;
    bool isPrecached(const std::string& id) const noexcept;

    const Plan& get(const std::string& id) const noexcept;
};
