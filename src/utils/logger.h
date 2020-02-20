#pragma once

#include <memory>
#include <ostream>

namespace smam
{
constexpr char cr  = '\n';
constexpr char tab = '\t';

enum class Col
{
    null   = -1,
    reset  = 0,
    red    = 31,
    green  = 32,
    yellow = 33,
    blue   = 34,
};

class Logger final
{
    std::ostream* os;

    bool color{true}, prefix{true};

public:
    Logger() noexcept;

    auto Print() noexcept -> Logger&;
    auto Info() noexcept -> Logger&;
    auto Warning() noexcept -> Logger&;
    auto Error() noexcept -> Logger&;
    auto Out() noexcept -> Logger&;

    void SetPrefix(bool) noexcept;
    void SetColor(bool) noexcept;
    void SetOutput(bool) noexcept;

    auto Parse(const Col&) -> std::string;

    auto operator<<(const Col&) -> Logger&;

    template <typename T>
    auto operator<<(const T& other) -> Logger&
    {
        *os << other;
        return *this;
    }
};

using LoggerPtr = std::shared_ptr<Logger>;
}  // namespace smam
