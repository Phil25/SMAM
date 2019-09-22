#pragma once

#include <data/addon.h>
#include <operations/core.hpp>

namespace smam
{
struct InstallerContext final
{
    std::map<std::string, AddonPlan> data;
    std::set<std::string>            pendingToBeInstalled;
};

class FetchData final : public Operation<InstallerContext>
{
    const std::string&              databaseUrl;
    const std::vector<std::string>& ids;

public:
    FetchData(Logger&, InstallerContext&,
              const std::string&              databaseUrl,
              const std::vector<std::string>& ids) noexcept;

    void Run() noexcept override;
};

class CheckPending final : public Operation<InstallerContext>
{
    std::string currentID;

public:
    CheckPending(Logger&, InstallerContext&,
                 std::string currentID) noexcept;

    void Run() noexcept override;
};
}  // namespace smam
