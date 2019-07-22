#pragma once

#include <filesystem>
#include <functional>
#include <map>
#include <set>

#include <smfs/file.h>

#include <nlohmann/json.hpp>

class Addon final : public std::enable_shared_from_this<Addon>
{
    using AddonPtr = std::shared_ptr<Addon>;
    using AddonSet = std::set<AddonPtr>;
    using AddonOpt = std::optional<AddonPtr>;

    using FileRef   = std::reference_wrapper<const File>;
    using FileTuple = std::pair<FileRef, std::string>;

    using EachAddon      = std::function<void(AddonPtr)>;
    using EachFile       = std::function<void(File&)>;
    using EachFileRemove = std::function<void(const FileTuple&)>;

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

    std::string           id, author, description;
    bool                  installedExplicitly;
    std::vector<File>     files;
    std::set<std::string> dependencies;

public:
    auto getId() const noexcept -> std::string;
    auto getAuthor() const noexcept -> std::string;
    auto getDescription() const noexcept -> std::string;
    bool isExplicit() const noexcept;
    auto getDeps() const noexcept -> const std::set<std::string>&;
    bool isInstalled() const noexcept;
    auto getFileCount() const noexcept -> size_t;

    bool install(const Scraper::Data&) noexcept;
    void addToInstalled() noexcept;
    void markExplicit() noexcept;

    void remove() noexcept;
    void remove(const EachFileRemove&) noexcept;
    void detach(const File&) noexcept;

    void forEachFile(const EachFile&) noexcept;

    static auto get(const std::string& id) noexcept -> AddonOpt;
    static bool isInstalled(const std::string& id) noexcept;

    static void forEach(const EachAddon&) noexcept;
    static auto findByFile(const File& file) noexcept -> AddonSet;

    [[nodiscard]] static auto load() noexcept -> LoadResult;
    [[nodiscard]] static bool save() noexcept;

    friend void from_json(const nlohmann::json&, Addon&);
    friend void to_json(nlohmann::json&, const Addon&) noexcept;
};

void from_json(const nlohmann::json&, Addon&);
void to_json(nlohmann::json&, const Addon&) noexcept;
