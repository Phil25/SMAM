#pragma once

#include <string>
#include <utils/codes.hpp>

namespace smam
{
struct Error final
{
    std::string message;
    ExitCode    code{ExitCode::OK};
    bool        silent{false};

    operator bool() const noexcept
    {
        return !message.empty() && !silent;
    }
};
}  // namespace smam
