#pragma once

#include <nlohmann/json.hpp>

#include <filesystem>
#include <string>

namespace smam
{
class File final
{
    std::string path, name, link;

public:
    File(const std::string&);

    auto Path() const noexcept -> const std::string&;
    void Path(std::string) noexcept;

    auto Name() const noexcept -> const std::string&;
    void Name(std::string) noexcept;

    auto Link() const noexcept -> const std::string&;
    void Link(std::string) noexcept;

    auto Raw() const noexcept -> std::string;
};

using FilePtr = std::shared_ptr<File>;

bool operator==(const FilePtr&, const FilePtr&) noexcept;
bool operator!=(const FilePtr&, const FilePtr&) noexcept;

void from_json(const nlohmann::json&, FilePtr&);
void to_json(nlohmann::json&, const FilePtr&) noexcept;
}  // namespace smam
