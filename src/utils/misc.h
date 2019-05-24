#pragma once

#include <string>
#include <vector>

namespace Utils
{
using StringVector = std::vector<std::string>;

// split string to vector by newlines
auto toLines(const std::string&) noexcept -> StringVector;

// returns whether given string is a link
bool isLink(const std::string&) noexcept;

// extract string from data between `from` and `to`
auto extract(const std::string& data, const std::string& from,
             const std::string& to) noexcept -> std::string;
}  // namespace Utils
