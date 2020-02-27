#pragma once

#include <data/file.h>

#include <set>

#ifndef NDEBUG
#include <utils/logger.h>
#endif

namespace smam
{
class Addon;
using AddonPtr = std::shared_ptr<Addon>;

using FileVector    = std::vector<FilePtr>;
using DependencySet = std::set<std::string>;

class Addon final : public std::enable_shared_from_this<Addon>
{
    using AddonOpt     = std::optional<AddonPtr>;
    using ForEachAddon = std::function<void(AddonPtr)>;

    using InstalledMap = std::map<std::string, AddonPtr>;
    static InstalledMap installed;

    const std::string id, author, description;

    std::string   baseUrl;
    bool          installedExplicitly{false};
    FileVector    files;
    DependencySet dependencies;

public:
    Addon(std::string id, std::string author,
          std::string description) noexcept;

    Addon(const Addon&) = delete;

    auto ID() const noexcept -> const std::string&;
    auto Author() const noexcept -> const std::string&;
    auto Description() const noexcept -> const std::string&;

    bool IsExplicit() const noexcept;
    void MarkExplicit() noexcept;

    auto BaseURL() const noexcept -> const std::string&;
    void BaseURL(std::string) noexcept;

    auto Files() const noexcept -> const FileVector&;
    void Files(FileVector) noexcept;

    auto Dependencies() const noexcept -> const DependencySet&;
    void Dependencies(DependencySet) noexcept;

    bool IsInstalled() const noexcept;
    void MarkInstalled() noexcept;
    void MarkUninstalled() noexcept;

    void AddFiles(FileVector) noexcept;
    void EraseNonExitentFiles() noexcept;

    static auto Get(const std::string& id) noexcept -> AddonOpt;
    static bool IsInstalled(const std::string& id) noexcept;
    static void ForEach(const ForEachAddon&) noexcept;
    static void EraseAll() noexcept;

    static int CountByOwnedFile(const FilePtr&) noexcept;

    [[nodiscard]] static bool Load(const std::string& file) noexcept;
    [[nodiscard]] static bool Save(const std::string& file) noexcept;
};

void from_json(const nlohmann::json&, AddonPtr&);
void to_json(nlohmann::json&, const AddonPtr&) noexcept;

using AddonMap    = std::map<std::string, AddonPtr>;
using AddonMapPtr = std::shared_ptr<AddonMap>;

#ifndef NDEBUG  // overloads for easy debug message printing
auto operator<<(Logger&, const Addon&) -> Logger&;
#endif
}  // namespace smam
