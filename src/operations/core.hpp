#pragma once

#include <utils/logger.h>

namespace smam
{
struct Error final
{
    std::string message;
    bool        silent{false};

    operator bool() const noexcept
    {
        return !message.empty() && !silent;
    }
};

template <class Context>
class Operation
{
    Logger&  logger;
    Context& context;
    Error    error;

protected:
    Operation(Logger& logger, Context& context) noexcept
        : logger(logger), context(context)
    {
    }

    auto GetLogger() noexcept -> Logger&
    {
        return logger;
    }

    auto GetContext() noexcept -> Context&
    {
        return context;
    }

    void Fail(std::string message) noexcept
    {
        error = {std::move(message), false};
    }

    void Stop() noexcept
    {
        error = {"_", true};  // error message not empty -> error exists
    }

public:
    virtual void Run() noexcept = 0;

    auto GetError() const noexcept
    {
        return error;
    }
};

template <class Context>
class Executor final
{
    Error   error;
    Logger& logger;
    Context context;

public:
    Executor(Logger& logger) noexcept : logger(logger)
    {
    }

    template <typename OP, typename... Args,
              typename = std::enable_if_t<
                  std::is_base_of_v<Operation<Context>, OP>>>
    [[nodiscard]] auto Run(Args... args) noexcept -> Executor&
    {
        if (!error.message.empty())
        {
            return *this;  // halt while in erroneous state
        }

        auto op = OP{logger, context, std::forward<Args>(args)...};
        op.Run();
        error = op.GetError();

        return *this;
    }

    [[nodiscard]] auto GetError() const noexcept
    {
        return error;
    }

    auto GetContext() const noexcept -> const Context&
    {
        return context;
    }
};
}  // namespace smam
