#include "report.h"

#include "../../utils/printer.h"

using StringVector = std::vector<std::string>;

namespace
{
struct PrinterData
{
    std::string prefix;
    Col         color;
};

const std::map<Report::Type, PrinterData> printer{
    {Report::Type::Installed, {"> Installed: ", Col::green}},
    {Report::Type::Skipped, {"> Skipped: ", Col::yellow}},
    {Report::Type::Failed, {"> Failed: ", Col::red}},
};
}  // namespace

void Report::insert(Type type, const std::string& addon) noexcept
{
    data[type].push_back(addon);
}

void Report::print(Type type) noexcept
{
    if (data[type].empty()) return;
    auto toPrint = printer.at(type);

    out() << toPrint.color << toPrint.prefix;

    for (const auto& addon : data[type])
    {
        out << addon << ' ';
    }

    out << Col::reset << cr;
}
