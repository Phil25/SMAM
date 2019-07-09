#include <gtest/gtest.h>

#include "../src/opts.h"

/*
 * Convert string into command line options and return Opts object,
 * simluating running through terminal with arguments
 */
Opts make(const std::string& cmd) noexcept
{
    std::vector<std::string> args{"smam"};
    std::string              arg;

    std::istringstream iss(cmd);
    while (iss >> arg) args.push_back(arg);

    int         size = args.size();
    const char* argv[size];

    for (int i = 0; i < size; ++i)
    {
        argv[i] = args[i].c_str();
    }

    return {size, argv};
}

TEST(OptsTest, InstallGeneric)
{
    const Opts opts = make("install rtd thriller tf2items");
    EXPECT_EQ("install", opts.getCommand());

    const auto& addons = opts.getAddons();
    ASSERT_EQ(3, addons.size());

    EXPECT_EQ("rtd", addons[0]);
    EXPECT_EQ("thriller", addons[1]);
    EXPECT_EQ("tf2items", addons[2]);
}

TEST(OptsTest, SingleOption)
{
    const Opts opts = make("remove rtd -h tf2items");

    EXPECT_EQ("remove", opts.getCommand());
    EXPECT_EQ(2, opts.getAddons().size());

    EXPECT_TRUE(opts.help());
    EXPECT_FALSE(opts.quiet());
    EXPECT_FALSE(opts.noColor());
}

TEST(OptsTest, MultipleOptions)
{
    const Opts opts = make("search -q tf2items --no-color");

    EXPECT_EQ("search", opts.getCommand());
    EXPECT_EQ(1, opts.getAddons().size());

    EXPECT_TRUE(opts.quiet());
    EXPECT_TRUE(opts.noColor());
    EXPECT_FALSE(opts.help());
}

TEST(OptsTest, LogPath)
{
    const Opts opts = make("info --log /home/log tf2items");

    EXPECT_EQ("info", opts.getCommand());
    EXPECT_EQ(1, opts.getAddons().size());

    EXPECT_FALSE(opts.getDestination());

    ASSERT_TRUE(opts.getLog());
    EXPECT_EQ("/home/log", opts.getLog());
}

TEST(OptsTest, DesinationPath)
{
    const Opts opts = make("info --destination /home/server");

    EXPECT_EQ("info", opts.getCommand());
    EXPECT_EQ(0, opts.getAddons().size());

    EXPECT_FALSE(opts.getLog());

    ASSERT_TRUE(opts.getDestination());
    EXPECT_EQ("/home/server", opts.getDestination());
}

TEST(OptsTest, DatabaseHost)
{
    const Opts opts = make("remove --db-url https://localhost:2121/");
    EXPECT_EQ("https://localhost:2121/", opts.getDbUrl());
}
