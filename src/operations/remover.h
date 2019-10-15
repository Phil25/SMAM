#pragma once

#include <data/addon.h>
#include <scrapers/scraper.h>
#include <operations/core.hpp>

namespace smam
{
struct RemoverContext final
{
    std::string id;
    AddonPtr    addon;

    explicit RemoverContext(std::string id) noexcept;
};

class CheckNotInstalled final : public Operation<RemoverContext>
{
public:
    CheckNotInstalled(const LoggerPtr&, RemoverContext&) noexcept;

    void Run() noexcept override;
};

class RemoveAddon final : public Operation<RemoverContext>
{
public:
    RemoveAddon(const LoggerPtr&, RemoverContext&) noexcept;

    void Run() noexcept override;
};

class RemoveDependencies final : public Operation<RemoverContext>
{
public:
    RemoveDependencies(const LoggerPtr&, RemoverContext&) noexcept;

    void Run() noexcept override;
};
}  // namespace smam
