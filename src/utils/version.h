#pragma once

#include "misc.h"

namespace Utils
{
class Version
{
    std::vector<int> nums = {};

public:
    Version(std::string version) noexcept;
    int compare(const Version& other) const noexcept;

    // return string representing biggest version value
    static auto biggest(const StringVector& versions) noexcept
        -> const std::string&;
};
}  // namespace Utils
