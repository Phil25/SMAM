#pragma once

#include <sstream>

namespace smam::download
{
struct Error final
{
    std::string message;

    operator bool() const noexcept;
};

auto Raw(const std::string& url, std::ostream&) noexcept -> Error;
auto Html(const std::string& url) noexcept -> std::stringstream;
auto File(const std::string& url, const char* dest) noexcept -> Error;
}  // namespace smam::download
