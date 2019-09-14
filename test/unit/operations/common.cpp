#include <operations/common.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filesystem>

namespace smam
{
class OperationsCommonTest : public ::testing::Test
{
protected:
    Logger                  logger;
    Executor<CommonContext> exec{logger};

    auto MakeOptions(std::string command) noexcept -> Options
    {
        std::vector<std::string> args{"smam"};
        std::string              arg;

        std::istringstream iss(std::move(command));
        while (iss >> arg) args.push_back(arg);

        int         size = args.size();
        const char* argv[size];

        for (int i = 0; i < size; ++i)
        {
            argv[i] = args[i].c_str();
        }

        return {size, argv, logger};
    }

    void SetUp() override
    {
        namespace fs = std::filesystem;
        fs::create_directories("mod/addons/sourcemod/plugins");
    }

    void TearDown() override
    {
        std::filesystem::remove_all("mod");
    }
};

TEST_F(OperationsCommonTest, CheckAddons)
{
    auto options1 = MakeOptions("install tf2items rtd");
    auto error1   = exec.Run<CheckAddons>(options1).GetError();

    ASSERT_FALSE(error1);

    auto options2 = MakeOptions("install -d abc");
    auto error2   = exec.Run<CheckAddons>(options2).GetError();

    ASSERT_TRUE(error2);
    EXPECT_EQ("No addons specified.", error2.message);
}

TEST_F(OperationsCommonTest, CheckSMRootNoOptions)
{
    auto error = exec.Run<CheckSMRoot>(MakeOptions("")).GetError();
    ASSERT_TRUE(error);
    EXPECT_EQ("SourceMod root not found.", error.message);
}

TEST_F(OperationsCommonTest, CheckSMRootNotFound)
{
    auto error = exec.Run<CheckSMRoot>(MakeOptions("-d .")).GetError();
    ASSERT_TRUE(error);
    EXPECT_EQ("SourceMod root not found.", error.message);
}

TEST_F(OperationsCommonTest, CheckSMRootFound)
{
    namespace fs               = std::filesystem;
    static const fs::path root = "mod/addons/sourcemod";

    auto error =
        exec.Run<CheckSMRoot>(MakeOptions("install tf2items -d ./mod/"))
            .GetError();

    ASSERT_FALSE(error);
    EXPECT_TRUE(fs::equivalent(root, exec.GetContext().root));
}
}  // namespace smam
