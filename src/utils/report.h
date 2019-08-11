#pragma once

#include <map>
#include <string>
#include <vector>

/*
 * Helper class for collecting and printing a report of the installation
 * process at the end.
 */
class Report final
{
public:
    enum class Type
    {
        Installed,  // installed
        Removed,    // removed
        Skipped,    // already installed
        Queued,     // already being installed
        Ignored,    // depenency forcefully not installed
        Dangling,   // depenency forcefully not removed
        Failed      // failed to install
    };

private:
    std::map<Type, std::vector<std::string>> data;
    std::vector<std::string>                 remarks;

public:
    void insert(Type, const std::string& addon) noexcept;

    void remark(const std::string& id, const std::string& dependency,
                Type) noexcept;

    void print(Type) const noexcept;
    void print() const noexcept;
};
