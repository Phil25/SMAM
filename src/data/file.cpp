#include "file.h"

namespace smam
{
File::File(const std::string& data)
{
    auto sz = data.size();
    if (sz < 3)
    {
        throw std::invalid_argument("Invalid file: " + data);
    }

    auto at = data.find_first_of(';');
    if (at == std::string::npos) at = data.find_last_of('/');

    if (!at || at == std::string::npos || at == sz - 1)
    {
        throw std::invalid_argument("Invalid file: " + data);
    }

    path = data.substr(0, at);
    name = data.substr(at + 1);

    if (!path.size() || !name.size())
    {
        throw std::invalid_argument("Invalid file: " + data);
    }

    if (path.at(path.size() - 1) == '/') path.pop_back();
}

auto File::Path() const noexcept -> const std::string&
{
    return path;
}

auto File::Name() const noexcept -> const std::string&
{
    return name;
}

auto File::Link() const noexcept -> const std::string&
{
    return link;
}

auto File::Raw() const noexcept -> std::string
{
    return path + '/' + name;
}

bool operator==(const FilePtr& f1, const FilePtr& f2) noexcept
{
    return f1->Raw() == f2->Raw();
}

bool operator!=(const FilePtr& f1, const FilePtr& f2) noexcept
{
    return !operator==(f1, f2);
}

void from_json(const nlohmann::json& json, FilePtr& file)
{
    file = std::make_unique<File>(json);
}

void to_json(nlohmann::json& json, const FilePtr& file) noexcept
{
    json = file->Raw();
}
}  // namespace smam
