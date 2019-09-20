#pragma once

#include <data/addon.h>
#include <operations/core.hpp>

namespace smam
{
struct InstallerContext final
{
    AddonPtr              addon;
    std::set<std::string> pendingToBeInstalled;
};

class InitializeScrapers final : public Operation<InstallerContext>
{
public:
    InitializeScrapers(Logger&, InstallerContext&) noexcept;

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
