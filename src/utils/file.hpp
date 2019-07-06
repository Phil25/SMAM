#pragma once

#include <string>

/*
 * Defines a destination path, filename and the URL from which
 * the file can be downloaded.
 */
class File final
{
    size_t at = std::string::npos;

public:
    std::string path, name, url;

    File(const std::string& data, const std::string& url = "") noexcept
        : at(findSplit(data)), url(url)
    {
        if (at == std::string::npos) return;
        path = data.substr(0, at);
        name = data.substr(at + 1);
    }

    bool valid() const noexcept { return at != std::string::npos; }
    void invalidate() noexcept { at = std::string::npos; }

private:
    static auto findSplit(const std::string& data) noexcept -> size_t
    {
        if (auto size = data.size(); size)
        {
            auto at = data.find_first_of(';');
            return at == 0 || at == size - 1 ? std::string::npos : at;
        }
        else
        {
            return std::string::npos;
        }
    }
};
