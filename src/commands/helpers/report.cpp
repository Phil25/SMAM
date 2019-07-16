#include "report.h"

#include <utils/printer.h>

using StringVector = std::vector<std::string>;

namespace
{
struct PrinterData final
{
    std::string prefix;
    Col         color;
};

const std::map<Report::Type, PrinterData> printer{
    {Report::Type::Installed, {"> Installed: ", Col::green}},
    {Report::Type::Removed, {"> Removed: ", Col::green}},
    {Report::Type::Skipped, {"> Skipped: ", Col::yellow}},
    {Report::Type::Failed, {"> Failed: ", Col::red}},
};
}  // namespace

void Report::insert(Type type, const std::string& addon) noexcept
{
    data[type].push_back(addon);
}

void Report::remark(const std::string& remark) noexcept
{
    if (!remark.empty()) remarks.push_back(remark);
}

void Report::print(Type type) const noexcept
{
    if (!data.count(type) || data.at(type).empty()) return;

    auto toPrint = printer.at(type);

    out() << toPrint.color << toPrint.prefix;

    for (const auto& addon : data.at(type))
    {
        out << addon << ' ';
    }

    out << Col::reset << cr;
}

void Report::print() const noexcept
{
    for (const auto& [type, _] : printer) print(type);

    if (!remarks.size()) return;

    out(Ch::Info) << "Remarks:" << cr;
    for (const auto& remark : remarks)
    {
        out() << remark << cr;
    }
}
