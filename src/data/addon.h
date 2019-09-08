#pragma once

#include <data/file.h>

#include <set>

namespace smam
{
class Addon;
using AddonPtr = std::shared_ptr<Addon>;

using FileVector    = std::vector<FilePtr>;
using DependencySet = std::set<std::string>;

class Addon final : std::enable_shared_from_this<Addon>
{
    using AddonOpt     = std::optional<AddonPtr>;
    using InstalledMap = std::map<std::string, AddonPtr>;
    using ForEachAddon = std::function<void(AddonPtr)>;

    static InstalledMap installed;

    const std::string id, author, description;

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

    auto Files() const noexcept -> const FileVector&;
    void Files(FileVector) noexcept;

    auto Dependencies() const noexcept -> const DependencySet&;
    void Dependencies(DependencySet) noexcept;

    bool IsInstalled() const noexcept;
    void MarkInstalled() noexcept;

    static auto Get(const std::string& id) noexcept -> AddonOpt;
    static bool IsInstalled(const std::string& id) noexcept;
    static void ForEach(const ForEachAddon&) noexcept;
};

void from_json(const nlohmann::json&, AddonPtr&);
void to_json(nlohmann::json&, const AddonPtr&) noexcept;
}  // namespace smam
