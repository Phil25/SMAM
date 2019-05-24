#pragma once

#include <map>

constexpr char cr = '\n';

enum class Ch
{
    Std,
    Info,
    Warn,
    Error
};  // channel

enum class Col
{
    reset  = 0,
    red    = 31,
    green  = 32,
    yellow = 33,
    blue   = 34,
};

extern auto operator<<(std::ostream& os, const Col& c) noexcept
    -> std::ostream&;

class Printer
{
    struct ChannelData
    {
        std::ostream*    out;
        std::string_view prefix;
        Col              color;
    };

    static std::map<Ch, ChannelData> chData;

public:
    bool colors = true;

    void quiet() noexcept;     // disable output entirely
    void noPrefix() noexcept;  // disable prefixes such as [E], etc

    auto getStream(Ch c = Ch::Std) noexcept -> std::ostream&;
    auto operator()(Ch c = Ch::Std) noexcept
        -> std::ostream&;  // change channel
};

extern Printer out;  // global console output
