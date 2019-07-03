#include "printer.h"

#include <iostream>

namespace
{
using Channels    = std::array<Ch, 4>;
using Defaultdata = std::array<Printer::ChannelData, 4>;

// Helper for iterating channels
constexpr Channels chs = {Ch::Std, Ch::Info, Ch::Warn, Ch::Error};

// Default data of Printer
constexpr Defaultdata def = {{
    {"    ", Col::null, &std::cout},
    {"[I] ", Col::blue, &std::cout},
    {"[W] ", Col::yellow, &std::clog},
    {"[E] ", Col::red, &std::cerr},
}};

static_assert(chs.size() == def.size());

// Stream consumer
struct final : public std::ostream
{
    template <typename T>
    void operator<<(const T&) noexcept
    {
    }
} nullStream;

}  // namespace

Printer::Printer(bool prefix, bool color, bool output) noexcept
{
    setPrefix(prefix);
    setColor(color);
    setOutput(output);
}

void Printer::setPrefix(bool b) noexcept
{
    int i = 0;
    for (Ch c : chs) data[c].prefix = b ? def[i++].prefix : "";
}

void Printer::setColor(bool b) noexcept
{
    int i = 0;
    for (Ch c : chs) data[c].color = b ? def[i++].color : Col::null;
    this->color = b;
}

void Printer::setOutput(bool b) noexcept
{
    int i = 0;
    for (Ch ch : chs) data[ch].out = b ? def[i++].out : &nullStream;
}

auto Printer::operator<<(const char c) noexcept -> Printer&
{
    *data[current].out << c;
    return *this;
}

auto Printer::operator<<(Col c) noexcept -> Printer&
{
    if (c == Col::null || !color) return *this;
    return *this << "\033[1;" << static_cast<int>(c) << 'm';
}

auto Printer::operator<<(Ch c) noexcept -> Printer&
{
    return *this << data[c].color << data[c].prefix << Col::reset;
}

auto Printer::operator()(Ch c) noexcept -> Printer&
{
    return this->operator<<(c);
}

Printer out;  // instantiate Printer global
