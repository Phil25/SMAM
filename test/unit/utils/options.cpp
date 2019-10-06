#include <utils/options.h>

#include <gtest/gtest.h>

namespace smam
{
class OperationsOptionsTest : public ::testing::Test
{
    LoggerPtr logger = std::make_shared<Logger>();

protected:
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
};

TEST_F(OperationsOptionsTest, InstallGeneric)
{
    const Options opts = MakeOptions("install rtd thriller tf2items");
    EXPECT_EQ("install", opts.Command());

    const auto& addons = opts.Addons();
    ASSERT_EQ(3, addons.size());

    EXPECT_EQ("rtd", addons[0]);
    EXPECT_EQ("thriller", addons[1]);
    EXPECT_EQ("tf2items", addons[2]);
}

TEST_F(OperationsOptionsTest, SingleOption)
{
    const Options opts = MakeOptions("remove rtd -h tf2items");

    EXPECT_EQ("remove", opts.Command());
    EXPECT_EQ(2, opts.Addons().size());

    EXPECT_TRUE(opts.Help());
    EXPECT_FALSE(opts.Quiet());
    EXPECT_FALSE(opts.NoColor());
}

TEST_F(OperationsOptionsTest, MultipleOptions)
{
    const Options opts = MakeOptions("search -q tf2items --no-color");

    EXPECT_EQ("search", opts.Command());
    EXPECT_EQ(1, opts.Addons().size());

    EXPECT_TRUE(opts.Quiet());
    EXPECT_TRUE(opts.NoColor());
    EXPECT_FALSE(opts.Help());
}

TEST_F(OperationsOptionsTest, LogPath)
{
    const Options opts = MakeOptions("info --log /home/log tf2items");

    EXPECT_EQ("info", opts.Command());
    EXPECT_EQ(1, opts.Addons().size());

    EXPECT_FALSE(opts.Destination());

    ASSERT_TRUE(opts.LogFile());
    EXPECT_EQ("/home/log", opts.LogFile());
}

TEST_F(OperationsOptionsTest, DesinationPath)
{
    const Options opts = MakeOptions("info --destination /home/server");

    EXPECT_EQ("info", opts.Command());
    EXPECT_EQ(0, opts.Addons().size());

    EXPECT_FALSE(opts.LogFile());

    ASSERT_TRUE(opts.Destination());
    EXPECT_EQ("/home/server", opts.Destination());
}

TEST_F(OperationsOptionsTest, DatabaseHost)
{
    const Options opts =
        MakeOptions("remove --db-url https://localhost:2121/");
    EXPECT_EQ("https://localhost:2121/", opts.DatabaseUrl());
}
}  // namespace smam
