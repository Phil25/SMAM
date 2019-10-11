#pragma once

#include <filesystem>
#include <functional>

namespace smam::archive
{
using EachFile = std::function<void(const std::filesystem::path&)>;

bool IsValidArchive(const std::filesystem::path&) noexcept;

bool Extract(const std::filesystem::path&, const EachFile&) noexcept;
}  // namespace smam::archive
