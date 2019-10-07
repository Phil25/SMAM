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
    OptionsPtr            options;

    explicit CommonContext(const OptionsPtr&) noexcept;
};

class CheckAddons final : public Operation<CommonContext>
{
public:
    CheckAddons(const LoggerPtr&, CommonContext&) noexcept;

    void Run() noexcept;
};

class CheckSMRoot final : public Operation<CommonContext>
{
public:
    CheckSMRoot(const LoggerPtr&, CommonContext&) noexcept;

    void Run() noexcept;
};

class LoadAddons final : public Operation<CommonContext>
{
    std::filesystem::path path;

public:
    LoadAddons(const LoggerPtr&, CommonContext&,
               std::filesystem::path path = {}) noexcept;

    void Run() noexcept;
};

class SaveAddons final : public Operation<CommonContext>
{
    std::filesystem::path path;

public:
    SaveAddons(const LoggerPtr&, CommonContext&,
               std::filesystem::path path) noexcept;

    void Run() noexcept;
};
}  // namespace smam
