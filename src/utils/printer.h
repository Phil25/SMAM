#pragma once

#include <map>

constexpr char cr = '\n';

enum class Ch
{
    Std,
    Info,
    Warn,
    Error
};

enum class Col
{
    null   = -1,
    reset  = 0,
    red    = 31,
    green  = 32,
    yellow = 33,
    blue   = 34,
};

class Printer
{
public:
    struct ChannelData
    {
        std::string_view prefix;
        Col              color;
        std::ostream*    out;
    };

    using DataMap = std::map<Ch, ChannelData>;

private:
    bool    color   = true;
    Ch      current = Ch::Std;
    DataMap data;

public:
    Printer(bool prefix = true, bool color = true,
            bool output = true) noexcept;

    void setPrefix(bool) noexcept;
    void setColor(bool) noexcept;
    void setOutput(bool) noexcept;

    auto operator<<(const char) noexcept -> Printer&;
    auto operator<<(Col) noexcept -> Printer&;  // change color
    auto operator<<(Ch) noexcept -> Printer&;   // change channel
    auto operator()(Ch = Ch::Std) noexcept -> Printer&;

    template <typename T>
    auto operator<<(const T& t) noexcept -> Printer&
    {
        *data[current].out << t;
        return *this;
    }
};

extern Printer out;  // global console output
