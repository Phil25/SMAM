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
        Failed      // failed to install/remove
    };

private:
    std::map<Type, std::vector<std::string>> data;
    std::vector<std::string>                 remarks;

public:
    void insert(Type type, const std::string& addon) noexcept;

    void remark(const std::string& id, const std::string& dependency,
                Type type) noexcept;

    void print(Type type) const noexcept;
    void print() const noexcept;
};
