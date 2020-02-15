#pragma once

#include <utils/logger.h>
#include <utils/error.hpp>

namespace smam
{
template <class Context>
class Operation
{
    LoggerPtr logger;
    Context&  context;
    Error     error;

protected:
    Operation(const LoggerPtr& logger, Context& context) noexcept
        : logger(logger), context(context)
    {
    }

    auto GetLogger() noexcept -> LoggerPtr&
    {
        return logger;
    }

    auto GetContext() noexcept -> Context&
    {
        return context;
    }

    void Fail(std::string msg, ExitCode code = ExitCode::OK) noexcept
    {
        error = {std::move(msg), code, false};
    }

    void Stop() noexcept
    {
        // error message not empty -> error exists
        error = {"_", ExitCode::OK, true};
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
    Error     error;
    LoggerPtr logger;
    Context   context;

public:
    template <typename... Args>
    Executor(const LoggerPtr& logger, Args... args) noexcept
        : logger(logger), context(std::forward<Args>(args)...)
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
