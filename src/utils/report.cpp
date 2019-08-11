#include "report.h"

#include <utils/printer.h>

using StringVector = std::vector<std::string>;

namespace
{
using Type = Report::Type;

struct PrinterData final
{
    std::string prefix, remark;
    Col         color;
};

const std::map<Type, PrinterData> printer{
    {Type::Installed, {"Installed", "installed", Col::green}},
    {Type::Removed, {"Removed", "removed", Col::green}},
    {Type::Skipped, {"Skipped", "already installed", Col::yellow}},
    {Type::Queued, {"Queued", "already being installed", Col::yellow}},
    {Type::Ignored, {"Ignored", "was not installed", Col::yellow}},
    {Type::Dangling, {"Dangling", "was not removed", Col::yellow}},
    {Type::Failed, {"Failed", "failed to install", Col::red}},
};

inline auto wrap(const std::string& addon) noexcept
{
    return out.parse(Col::green) + addon + out.parse(Col::reset);
}
}  // namespace

void Report::insert(Type type, const std::string& addon) noexcept
{
    data[type].push_back(addon);
}

void Report::remark(const std::string& id, const std::string& dep,
                    Type type) noexcept
{
    remarks.emplace_back("Dependency " + wrap(dep) + " of " + wrap(id) +
                         ' ' + printer.at(type).remark + '.');
}

void Report::print(Type type) const noexcept
{
    if (!data.count(type) || data.at(type).empty()) return;

    auto toPrint = printer.at(type);

    out() << toPrint.color << "> " << toPrint.prefix << ": ";

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
    for (const auto& remark : remarks) out() << remark << cr;
}
