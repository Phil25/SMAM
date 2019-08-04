#pragma once

#include <string>

namespace Download
{
auto page(const std::string& url, const std::string& from = "",
          const std::string& to = "") noexcept -> std::string;

auto file(const std::string& url, const std::string& dest) noexcept
    -> std::string;

void placeFiles() noexcept;
};  // namespace Download
