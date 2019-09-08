#include "addon.h"

namespace smam
{
Addon::InstalledMap Addon::installed;

Addon::Addon(std::string id, std::string author,
             std::string description) noexcept
    : id(std::move(id)), author(author), description(description)
{
}

auto Addon::ID() const noexcept -> const std::string&
{
    return id;
}

auto Addon::Author() const noexcept -> const std::string&
{
    return author;
}

auto Addon::Description() const noexcept -> const std::string&
{
    return description;
}

bool Addon::IsExplicit() const noexcept
{
    return installedExplicitly;
}

void Addon::MarkExplicit() noexcept
{
    installedExplicitly = true;
}

auto Addon::Files() const noexcept -> const FileVector&
{
    return files;
}

void Addon::Files(FileVector files) noexcept
{
    this->files = std::move(files);
}

auto Addon::Dependencies() const noexcept -> const DependencySet&
{
    return dependencies;
}

void Addon::Dependencies(DependencySet dependencies) noexcept
{
    this->dependencies = std::move(dependencies);
}

bool Addon::IsInstalled() const noexcept
{
    return Addon::IsInstalled(id);
}

void Addon::MarkInstalled() noexcept
{
    installed.emplace(id, shared_from_this());
}

/*static*/ auto Addon::Get(const std::string& id) noexcept -> AddonOpt
{
    if (Addon::IsInstalled(id)) return installed.at(id);
    return std::nullopt;
}

/*static*/ bool Addon::IsInstalled(const std::string& id) noexcept
{
    return installed.count(id);
}

/*static*/ void Addon::ForEach(const ForEachAddon& f) noexcept
{
    for (const auto& [_, addon] : installed) f(addon);
}

void from_json(const nlohmann::json& json, AddonPtr& addon)
{
    addon = std::make_shared<Addon>(json.at("id"), json.at("author"),
                                    json.at("description"));

    if (json.value("explicit", false))
    {
        addon->MarkExplicit();
    }

    if (auto it = json.find("files"); it != json.end())
    {
        auto files = FileVector();
        it->get_to(files);
        addon->Files(std::move(files));
    }

    if (auto it = json.find("deps"); it != json.end())
    {
        auto dependencies = DependencySet();
        it->get_to(dependencies);
        addon->Dependencies(std::move(dependencies));
    }
}

void to_json(nlohmann::json& json, const AddonPtr& addon) noexcept
{
    json.emplace("id", addon->ID());
    json.emplace("author", addon->Author());
    json.emplace("description", addon->Description());
    json.emplace("explicit", addon->IsExplicit());
    json.emplace("files", addon->Files());
    json.emplace("deps", addon->Dependencies());
}
}  // namespace smam
