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
        Installed,
        Removed,
        Skipped,
        Failed
    };

private:
    std::map<Type, std::vector<std::string>> data;

public:
    void insert(Type type, const std::string& addon) noexcept;
    void print(Type type) noexcept;
    void print() noexcept;
};
