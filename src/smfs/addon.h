#pragma once

#include <filesystem>
#include <functional>
#include <map>
#include <set>

#include <smfs/file.h>

#include <nlohmann/json.hpp>

class Addon;
using AddonPtr = std::shared_ptr<Addon>;

class Addon final : public std::enable_shared_from_this<Addon>
{
    using AddonSet = std::set<AddonPtr>;
    using AddonOpt = std::optional<AddonPtr>;

    using EachAddon = std::function<void(AddonPtr)>;
    using EachFile  = std::function<bool(const FilePtr&)>;
    using EachDep   = std::function<bool(const std::string&)>;

    using EachFileRemove =
        std::function<void(const FilePtr&, const std::string& error)>;

    using InstalledMap = std::map<std::string, AddonPtr>;

public:
    enum class LoadResult
    {
        OK,
        NoAccess,
        Corrupted
    };

private:
    static InstalledMap installed;

    const std::string id;

    std::string           author, description;
    bool                  installedExplicitly;
    std::vector<FilePtr>  files;
    std::set<std::string> dependencies;

public:
    Addon(const std::string& id);
    Addon(const Addon&) = delete;

    auto getId() const noexcept -> std::string;
    auto getAuthor() const noexcept -> std::string;
    auto getDescription() const noexcept -> std::string;
    auto fileCount() const noexcept -> size_t;

    bool isExplicit() const noexcept;
    void markExplicit() noexcept;

    bool install(const Scraper::Data&) noexcept;
    void addToInstalled() noexcept;
    bool isInstalled() const noexcept;

    void remove() noexcept;
    void remove(const EachFileRemove&) noexcept;
    void detach(const FilePtr&) noexcept;

    void appendFiles(const std::vector<FilePtr>&) noexcept;
    bool forEachFile(const EachFile&) noexcept;
    bool forEachDep(const EachDep&) noexcept;

    static auto get(const std::string& id) noexcept -> AddonOpt;
    static bool isInstalled(const std::string& id) noexcept;

    static void forEach(const EachAddon&) noexcept;
    static auto findByFile(const FilePtr& file) noexcept -> AddonSet;
    static auto findByDep(const std::string& dep) noexcept -> AddonSet;

    static void               clear() noexcept;
    [[nodiscard]] static auto load() noexcept -> LoadResult;
    [[nodiscard]] static bool save() noexcept;

    friend void from_json(const nlohmann::json&, AddonPtr&);
    friend void to_json(nlohmann::json&, const AddonPtr&) noexcept;
};

void from_json(const nlohmann::json&, AddonPtr&);
void to_json(nlohmann::json&, const AddonPtr&) noexcept;
