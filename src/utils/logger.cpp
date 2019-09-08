#include "logger.h"

#include <iostream>

namespace
{
// Stream consumer
struct final : public std::ostream
{
    template <typename T>
    void operator<<(const T&) noexcept
    {
    }
} nullStream;
}  // namespace

namespace smam
{
Logger::Logger() noexcept : os(&std::cout)
{
}

auto Logger::operator()() noexcept -> Logger&
{
    if (prefix) *this << "    ";
    return *this;
}

auto Logger::Info() noexcept -> Logger&
{
    *this << Col::blue;
    if (prefix) *this << "[I] ";
    return *this << Col::reset;
}

auto Logger::Warning() noexcept -> Logger&
{
    *this << Col::yellow;
    if (prefix) *this << "[W] ";
    return *this << Col::reset;
}

auto Logger::Error() noexcept -> Logger&
{
    *this << Col::red;
    if (prefix) *this << "[E] ";
    return *this << Col::reset;
}

void Logger::SetPrefix(bool prefix) noexcept
{
    this->prefix = prefix;
}

void Logger::SetColor(bool color) noexcept
{
    this->color = color;
}

void Logger::SetOutput(bool output) noexcept
{
    this->os = output ? &std::cout : &nullStream;
}

auto Logger::Parse(const Col& c) -> std::string
{
    if (c == Col::null || !color) return {};
    return "\033[1;" + std::to_string(static_cast<int>(c)) + 'm';
}

auto Logger::operator<<(const Col& c) -> Logger&
{
    return *this << Parse(c);
}
}  // namespace smam
