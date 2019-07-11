#include "version.h"

#include <algorithm>
#include <sstream>

Utils::Version::Version(std::string version) noexcept
{
    // turn every non-digit character to a space
    for (char& c : version)
    {
        if (!std::isdigit(c))
        {
            c = ' ';
        }
    }

    std::istringstream iss(version);
    std::string        word;

    // parse every word (space separated) into the int vector
    while (iss >> word)
    {
        nums.push_back(std::stoi(word));
    }
}

int Utils::Version::compare(const Version& other) const noexcept
{
    int len = std::min(nums.size(), other.nums.size());

    for (int i = 0; i < len; ++i)
    {
        if (nums[i] > other.nums[i])
        {
            return 1;
        }

        if (nums[i] < other.nums[i])
        {
            return -1;
        }
    }

    return 0;
}

auto Utils::Version::biggest(
    const std::vector<std::string>& versions) noexcept
    -> const std::string&
{
    int size = versions.size();
    if (size < 2) return versions[0];

    int     biggestIndex = 0;
    Version biggestVer   = Version(versions[0]);

    for (int i = 1; i < size; ++i)
    {
        Version ver = Version(versions[i]);
        if (ver.compare(biggestVer) == 1)
        {
            biggestVer   = ver;
            biggestIndex = i;
        }
    }

    return versions[biggestIndex];
}
