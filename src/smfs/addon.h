#pragma once

#include <filesystem>
#include <functional>
#include <map>
#include <set>

#include <smfs/file.h>

#include <nlohmann/json.hpp>

class Addon final : public std::enable_shared_from_this<Addon>
{
    using Ptr      = std::shared_ptr<Addon>;
    using AddonSet = std::set<Ptr>;
    using AddonOpt = std::optional<Ptr>;

    using FileTuple = std::pair<File::Ptr, std::string>;

    using EachAddon = std::function<void(Ptr)>;
    using EachFile  = std::function<bool(const File::Ptr&)>;
    using EachDep   = std::function<bool(const std::string&)>;

    using EachFileRemove =
        std::function<void(const File::Ptr&, const std::string& error)>;

    using InstalledMap = std::map<std::string, std::shared_ptr<Addon>>;

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

    std::string            author, description;
    bool                   installedExplicitly;
    std::vector<File::Ptr> files;
    std::set<std::string>  dependencies;

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
    void detach(const File::Ptr&) noexcept;

    void appendFiles(const std::vector<File::Ptr>&) noexcept;
    bool forEachFile(const EachFile&) noexcept;
    bool forEachDep(const EachDep&) noexcept;

    static auto get(const std::string& id) noexcept -> AddonOpt;
    static bool isInstalled(const std::string& id) noexcept;

    static void forEach(const EachAddon&) noexcept;
    static auto findByFile(const File::Ptr& file) noexcept -> AddonSet;

    static void               clear() noexcept;
    [[nodiscard]] static auto load() noexcept -> LoadResult;
    [[nodiscard]] static bool save() noexcept;

    friend void from_json(const nlohmann::json&, Ptr&);
    friend void to_json(nlohmann::json&, const Ptr&) noexcept;
};

void from_json(const nlohmann::json&, Addon::Ptr&);
void to_json(nlohmann::json&, const Addon::Ptr&) noexcept;
