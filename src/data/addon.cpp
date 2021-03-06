#include "addon.h"

#include <nlohmann/json.hpp>

#include <filesystem>
#include <fstream>

namespace smam
{
Addon::InstalledMap Addon::installed;
DependencySet       Addon::markedForRemoval;

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

auto Addon::BaseURL() const noexcept -> const std::string&
{
    return baseUrl;
}

void Addon::BaseURL(std::string baseUrl) noexcept
{
    this->baseUrl = std::move(baseUrl);
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

void Addon::MarkUninstalled() noexcept
{
    installed.erase(id);
}

bool Addon::IsMarkedForRemoval() const noexcept
{
    return markedForRemoval.count(this->id);
}

void Addon::MarkForRemoval() noexcept
{
    markedForRemoval.insert(this->id);
}

void Addon::AddFiles(FileVector toAdd) noexcept
{
    files.insert(files.end(), std::make_move_iterator(toAdd.begin()),
                 std::make_move_iterator(toAdd.end()));
}

void Addon::EraseNonExitentFiles() noexcept
{
    namespace fs = std::filesystem;

    auto it = files.begin();
    while (it != files.end())
    {
        if (auto path = fs::path{(*it)->Raw()}; fs::exists(path))
        {
            ++it;
        }
        else
        {
            assert(!fs::is_directory(path));
            it = files.erase(it);
        }
    }
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

/*static*/ void Addon::EraseAll() noexcept
{
    installed.clear();
}

/*static*/ int Addon::CountByOwnedFile(const FilePtr& file) noexcept
{
    return std::count_if(
        installed.begin(), installed.end(), [&file](const auto& entry) {
            const auto& f = entry.second->Files();
            return std::find(f.begin(), f.end(), file) != f.end();
        });
}

/*static*/ int Addon::CountByDependency(const std::string& id) noexcept
{
    return std::count_if(
        installed.begin(), installed.end(), [&id](const auto& entry) {
            for (const auto& dep : entry.second->Dependencies())
            {
                if (dep == id)
                    return !markedForRemoval.count(entry.first);
            }
            return false;
        });
}

/*static*/ bool Addon::Load(const std::string& file) noexcept
{
    auto ifs = std::ifstream(file);
    if (!ifs) return true;  // not exists, nothing to load

    auto   addons = std::vector<AddonPtr>();
    auto   json   = nlohmann::json();
    size_t hash;

    try
    {
        ifs >> json;
        json.at("data").get_to(addons);
        hash = json.at("hash").get<size_t>();
    }
    catch (const nlohmann::json::exception&)
    {
        return false;
    }

    if (hash != std::hash<nlohmann::json>{}(json["data"]))
    {
        return false;
    }

    for (auto& addon : addons) addon->MarkInstalled();

    ifs.close();
    return true;
}

/*static*/ bool Addon::Save(const std::string& file) noexcept
{
    auto ofs = std::ofstream(file, std::ios::trunc);
    if (!ofs) return false;

    auto addons = std::vector<AddonPtr>();
    for (const auto& [_, addon] : installed) addons.push_back(addon);

    auto json    = nlohmann::json();
    json["data"] = std::move(addons);
    json["hash"] = std::hash<nlohmann::json>{}(json["data"]);

    ofs << json;

    ofs.close();
    return true;
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

#ifndef NDEBUG  // overloads for easy debug message printing
auto operator<<(Logger& logger, const Addon& addon) -> Logger&
{
    logger << '{' << cr;  // no Indent() because same line
    logger.IncIndent();
    logger.Indent() << "name: " << addon.ID() << cr;
    logger.Indent() << "description: " << addon.Description() << cr;
    logger.Indent() << "author: " << addon.Author() << cr;
    logger.Indent() << "baseURL: \"" << addon.BaseURL() << '\"' << cr;
    logger.Indent() << "files: " << addon.Files() << cr;
    logger.Indent() << "dependencies: " << addon.Dependencies() << cr;
    logger.Indent() << "explicit: " << addon.IsExplicit() << cr;
    logger.Indent() << "installed: " << addon.IsInstalled() << cr;
    logger.DecIndent();
    return logger.Indent() << '}';
}
#endif
}  // namespace smam
