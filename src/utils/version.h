#pragma once

#include <string>
#include <vector>

namespace Utils
{
class Version final
{
    std::vector<int> nums = {};

public:
    Version(std::string version) noexcept;
    int compare(const Version& other) const noexcept;

    // return string representing biggest version value
    static auto biggest(
        const std::vector<std::string>& versions) noexcept
        -> const std::string&;
};
}  // namespace Utils
