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

public:
    enum class LoadResult
    {
        OK,
        NoAccess,
        Corrupted
    };

private:
    static std::map<std::string, std::shared_ptr<Addon>> installed;

    std::vector<File> files;
    bool              installedExplicitly;

public:
    const std::string id;

    std::string           author, description;
    std::set<std::string> dependencies;

    Addon(const std::string& id, const std::vector<File>& files,
          const std::string&           author       = "",
          const std::string&           description  = "",
          const std::set<std::string>& dependencies = {});

    ~Addon() noexcept;

    bool install(const Scraper::Data&) noexcept;
    void remove() noexcept;
    void remove(const EachFileRemove&) noexcept;
    void detach(const File&) noexcept;

    void markExplicit() noexcept;
    bool isExplicit() const noexcept;
    bool isInstalled() const noexcept;
    auto getFileCount() const noexcept -> size_t;

    void forEachFile(const EachFile&) noexcept;

    static auto get(const std::string& id) noexcept -> AddonOpt;
    static bool isInstalled(const std::string& id) noexcept;

    static void forEach(const EachAddon&) noexcept;
    static auto findByFile(const File& file) noexcept -> AddonSet;

    [[nodiscard]] static auto load() noexcept -> LoadResult;
    [[nodiscard]] static bool save() noexcept;

    friend nlohmann::adl_serializer<Addon>;
};

namespace nlohmann
{
template <>
struct adl_serializer<Addon>
{
    static auto from_json(const nlohmann::json& j) -> Addon;
    static void to_json(json& j, const Addon&) noexcept;
};
}  // namespace nlohmann
