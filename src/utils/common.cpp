#include "common.h"

#include <cassert>
#include <sstream>

namespace smam::utils
{
auto SplitLines(const std::string& data) noexcept
    -> std::vector<std::string>
{
    std::istringstream       dataStream(data);
    std::string              line;
    std::vector<std::string> lines;

    while (std::getline(dataStream, line))
    {
        lines.push_back(line);
    }

    return lines;
}

bool IsLink(const std::string& str) noexcept
{
    static constexpr std::string_view https = "https://";
    static constexpr std::string_view http  = "http://";

    return str.compare(0, https.size(), https) == 0 ||
           str.compare(0, http.size(), http) == 0;
}

auto ExtractString(std::string source, const std::string& from,
                   const std::string& to) noexcept -> std::string
{
    if (from.empty() || to.empty()) return source;

    auto start = source.find(from) + from.size();
    auto end   = source.find(to, start);

    return source.substr(start, end - start);
}

class Version final
{
    std::vector<int> nums;

public:
    Version(std::string version) noexcept
    {
        // turn every non-digit character to a space
        for (char& c : version)
        {
            if (!std::isdigit(c))
            {
                c = ' ';
            }
        }

        std::istringstream iss(std::move(version));
        std::string        word;

        // parse every word (space separated) into the int vector
        while (iss >> word)
        {
            nums.push_back(std::stoi(word));
        }
    }

    int Compare(const Version& other) const noexcept
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
};

auto BiggestVersion(const std::vector<std::string>& versions) noexcept
    -> const std::string&
{
    assert(versions.size());

    int  biggestIndex = 0;
    auto biggestVer   = Version(versions[0]);

    for (size_t i = 1; i < versions.size(); ++i)
    {
        auto v = Version(versions[i]);

        if (v.Compare(biggestVer) == 1)
        {
            biggestVer   = v;
            biggestIndex = i;
        }
    }

    return versions[biggestIndex];
}
}  // namespace smam::utils
