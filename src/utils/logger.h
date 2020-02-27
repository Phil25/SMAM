#pragma once

#include <memory>
#include <ostream>

#ifndef NDEBUG  // overloads for easy debug message printing
#include <vector>
#include <set>
#include <map>
#include <optional>
#endif

namespace smam
{
constexpr char cr  = '\n';
constexpr char tab = '\t';

enum class Col
{
    null    = -1,
    reset   = 0,
    red     = 31,
    green   = 32,
    yellow  = 33,
    blue    = 34,
    magenta = 35,
    cyan    = 36,
};

#define VAR(name) Col::cyan, #name, Col::reset, '=', name

class Logger final
{
    std::ostream* os;

    bool color{true}, prefix{true};

#ifndef NDEBUG
    short indent{0};
#endif

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

    template <typename... Args>
    inline void Debug(Args&&... args) noexcept
    {
#ifndef NDEBUG
        if (prefix) *this << Col::magenta << "[D] " << Col::reset;
        ((*this << std::forward<Args>(args)), ...);
        *this << cr;
#endif
    }

    template <typename T>
    auto operator<<(const T& other) -> Logger&
    {
        *os << other;
        return *this;
    }

#ifndef NDEBUG
    void IncIndent() noexcept;
    void DecIndent() noexcept;
    auto Indent() noexcept -> Logger&;

    // overloads for easy debug message printing

    template <typename T>
    auto operator<<(const std::vector<T>& vec) -> Logger&
    {
        if (vec.empty()) return *this << "[]";

        auto it = vec.cbegin();

        *this << '[' << *it;
        while (++it != vec.cend())
        {
            *this << ", " << *it;
        }

        return *this << ']';
    }

    template <typename T>
    auto operator<<(const std::set<T>& set) -> Logger&
    {
        if (set.empty()) return *this << "{}";

        auto it = set.cbegin();

        *this << '{' << *it;
        while (++it != set.cend())
        {
            *this << ", " << *it;
        }

        return *this << '}';
    }

    template <typename K, typename V>
    auto operator<<(const std::map<K, V>& map) -> Logger&
    {
        if (map.empty()) return *this << "{}";

        *this << '{';  // no Indent() because same line
        this->IncIndent();

        for (const auto& [k, v] : map)
        {
            *this << cr;
            this->Indent() << k << ": " << v;
        }

        *this << cr;
        this->DecIndent();

        return this->Indent() << '}';
    }

    template <typename T>
    auto operator<<(const std::shared_ptr<T>& ptr) -> Logger&
    {
        if (!ptr) return *this << "(nullptr)";
        return *this << *ptr;
    }

    template <typename T>
    auto operator<<(const std::optional<T>& opt) -> Logger&
    {
        if (!opt) return *this << "(nullopt)";
        return *this << *opt;
    }
#endif
};

using LoggerPtr = std::shared_ptr<Logger>;
}  // namespace smam
