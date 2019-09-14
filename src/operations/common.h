#pragma once

#include <utils/options.h>
#include <operations/core.hpp>

#include <filesystem>
#include <vector>

namespace smam
{
struct CommonContext final
{
    std::filesystem::path root;
};

class CheckAddons final : public Operation<CommonContext>
{
    const Options& options;

public:
    CheckAddons(Logger&, CommonContext&, const Options&) noexcept;

    void Run() noexcept;
};

class CheckSMRoot final : public Operation<CommonContext>
{
    const Options& options;

public:
    CheckSMRoot(Logger&, CommonContext&, const Options&) noexcept;

    void Run() noexcept;
};
}  // namespace smam
