#pragma once

#include <data/addon.h>
#include <operations/common.hpp>

namespace smam
{
struct AddonContext
{
    AddonPtr              addon;
    std::set<std::string> pendingToBeInstalled;
};

class CheckPending final : public Operation<AddonContext>
{
    std::string currentID;

public:
    CheckPending(Logger&, AddonContext&,
                 std::string currentID) noexcept;

    void Run() noexcept override;
};
}  // namespace smam