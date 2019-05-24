#include "printer.h"

#include <iostream>

auto operator<<(std::ostream& os, const Col& c) noexcept
    -> std::ostream&
{
    if (!out.colors) return os;
    return os << "\033[1;" << static_cast<int>(c) << 'm';
}

namespace
{
// Helper for iterating channels
constexpr std::array<Ch, 4> chs = {Ch::Std, Ch::Info, Ch::Warn,
                                   Ch::Error};
}  // namespace

void Printer::quiet() noexcept
{
    for (auto& c : chs) chData[c].out->rdbuf(nullptr);
}

void Printer::noPrefix() noexcept
{
    for (auto& c : chs) chData[c].prefix = "";
}

auto Printer::operator()(Ch c) noexcept -> std::ostream&
{
    *chData[c].out << chData[c].color;
    *chData[c].out << chData[c].prefix;
    *chData[c].out << Col::reset;

    return *chData[c].out;
}

auto Printer::getStream(Ch c) noexcept -> std::ostream&
{
    return *chData[c].out;
}

std::map<Ch, Printer::ChannelData> Printer::chData{
    {Ch::Std, {&std::cout, "    ", Col::reset}},
    {Ch::Info, {&std::cout, "[I] ", Col::blue}},
    {Ch::Warn, {&std::clog, "[W] ", Col::yellow}},
    {Ch::Error, {&std::cerr, "[E] ", Col::red}}};

Printer out;  // instantiate Printer global
