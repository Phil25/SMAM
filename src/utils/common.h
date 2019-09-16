#pragma once

#include <string>
#include <vector>

namespace smam::utils
{
auto SplitLines(const std::string& data) noexcept
    -> std::vector<std::string>;

bool IsLink(const std::string& str) noexcept;

auto ExtractString(std::string source, const std::string& from,
                   const std::string& to) noexcept -> std::string;

auto BiggestVersion(const std::vector<std::string>&) noexcept
    -> const std::string&;
}  // namespace smam::utils
