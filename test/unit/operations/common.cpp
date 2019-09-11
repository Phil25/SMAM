#include <operations/common.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace smam
{
struct TestContext final
{
    int step{0};
    int value{0};
};

class OperationBlank final : public Operation<TestContext>
{
public:
    OperationBlank(Logger& logger, TestContext& context) noexcept
        : Operation(logger, context)
    {
    }

    void Run() noexcept override
    {
        ++GetContext().step;
    }
};

class OperationStopping final : public Operation<TestContext>
{
public:
    OperationStopping(Logger& logger, TestContext& context) noexcept
        : Operation(logger, context)
    {
    }

    void Run() noexcept override
    {
        Stop();
    }
};

class OperationFailing final : public Operation<TestContext>
{
public:
    OperationFailing(Logger& logger, TestContext& context) noexcept
        : Operation(logger, context)
    {
    }

    void Run() noexcept override
    {
        Fail("failure");
    }
};

class OperationAdder final : public Operation<TestContext>
{
    int valueToAdd;

public:
    OperationAdder(Logger& logger, TestContext& context, int v) noexcept
        : Operation(logger, context), valueToAdd(v)
    {
    }

    void Run() noexcept override
    {
        ++GetContext().step;
        GetContext().value += valueToAdd;
    }
};

class OperationsCommonTest : public ::testing::Test
{
protected:
    Logger                logger;
    Executor<TestContext> exec{logger};
};

TEST_F(OperationsCommonTest, SingleFinish)
{
    auto error = exec.Run<OperationBlank>().GetError();
    EXPECT_FALSE(error);
    EXPECT_EQ(1, exec.GetContext().step);
}

TEST_F(OperationsCommonTest, ChainFinish)
{
    auto error = exec.Run<OperationBlank>()
                     .Run<OperationBlank>()
                     .Run<OperationBlank>()
                     .Run<OperationBlank>()
                     .Run<OperationBlank>()
                     .Run<OperationBlank>()
                     .Run<OperationBlank>()
                     .Run<OperationBlank>()
                     .Run<OperationBlank>()
                     .GetError();

    EXPECT_FALSE(error);
    EXPECT_EQ(9, exec.GetContext().step);
}

TEST_F(OperationsCommonTest, ChainStop)
{
    auto error = exec.Run<OperationBlank>()
                     .Run<OperationBlank>()
                     .Run<OperationBlank>()
                     .Run<OperationBlank>()
                     .Run<OperationBlank>()
                     .Run<OperationStopping>()
                     .Run<OperationBlank>()
                     .Run<OperationBlank>()
                     .Run<OperationBlank>()
                     .GetError();

    EXPECT_FALSE(error);
    EXPECT_EQ(5, exec.GetContext().step);
}

TEST_F(OperationsCommonTest, ChainFail)
{
    auto error = exec.Run<OperationBlank>()
                     .Run<OperationBlank>()
                     .Run<OperationBlank>()
                     .Run<OperationFailing>()
                     .Run<OperationBlank>()
                     .Run<OperationBlank>()
                     .Run<OperationBlank>()
                     .Run<OperationBlank>()
                     .Run<OperationBlank>()
                     .GetError();

    ASSERT_TRUE(error);
    EXPECT_EQ(3, exec.GetContext().step);
    EXPECT_EQ("failure", error.message);
}

TEST_F(OperationsCommonTest, ChainAdder)
{
    auto error = exec.Run<OperationBlank>()
                     .Run<OperationAdder>(1)
                     .Run<OperationBlank>()
                     .Run<OperationAdder>(5)
                     .Run<OperationBlank>()
                     .Run<OperationAdder>(-2)
                     .Run<OperationAdder>(1)
                     .Run<OperationBlank>()
                     .GetError();

    EXPECT_FALSE(error);
    EXPECT_EQ(8, exec.GetContext().step);
    EXPECT_EQ(5, exec.GetContext().value);
}
}  // namespace smam
