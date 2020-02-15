#include <utils/path.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <fstream>

namespace smam
{
TEST(PathTest, PathIsSafe)
{
    EXPECT_TRUE(path::IsSafe(""));
    EXPECT_TRUE(path::IsSafe("."));
    EXPECT_TRUE(path::IsSafe("./"));
    EXPECT_TRUE(path::IsSafe(".."));
    EXPECT_TRUE(path::IsSafe("../"));
    EXPECT_TRUE(path::IsSafe("../."));
    EXPECT_TRUE(path::IsSafe(".././"));
    EXPECT_TRUE(path::IsSafe("../.."));
    EXPECT_TRUE(path::IsSafe("../../"));
    EXPECT_TRUE(path::IsSafe("../../."));
    EXPECT_TRUE(path::IsSafe("../.././"));

    EXPECT_FALSE(path::IsSafe("../../.."));
    EXPECT_FALSE(path::IsSafe("../../../"));
    EXPECT_FALSE(path::IsSafe("../../../."));
    EXPECT_FALSE(path::IsSafe("../../.././"));
    EXPECT_FALSE(path::IsSafe("../../../.."));

    EXPECT_FALSE(path::IsSafe(".././../.."));
    EXPECT_FALSE(path::IsSafe("../.././../"));
    EXPECT_FALSE(path::IsSafe(".././.././../."));
    EXPECT_FALSE(path::IsSafe(".././../../././"));
    EXPECT_FALSE(path::IsSafe("../../.././././.."));
}

TEST(PathTest, PathFindSMRoot)
{
    namespace fs = std::filesystem;

    fs::remove_all("mod");
    ASSERT_TRUE(fs::create_directories("mod/addons/sourcemod/plugins"));

    const fs::path root = "mod/addons/sourcemod";

    auto check = [&root](const fs::path& startFrom) {
        auto mp = path::FindSMRoot(startFrom);
        return mp.has_value() && fs::equivalent(mp.value(), root);
    };

    EXPECT_FALSE(path::FindSMRoot({}));
    EXPECT_FALSE(check(""));
    EXPECT_FALSE(check("."));
    EXPECT_FALSE(check("./"));

    EXPECT_TRUE(check("./mod"));
    EXPECT_TRUE(check("./mod/addons"));
    EXPECT_TRUE(check("./mod/addons/sourcemod"));
    EXPECT_TRUE(check("./mod/addons/sourcemod/plugins"));

    fs::create_directories("./mod/addons/sourcemod/plugins");
    fs::remove_all("mod");
}

TEST(PathTest, PathRemoveEmptyDirectories)
{
    namespace fs = std::filesystem;

    fs::remove_all("something");
    fs::create_directories("something/this/way/comes");
    fs::create_directories("something/this/somedir");

    path::RemoveEmptyDirectories("something/this/way/comes");

    EXPECT_FALSE(fs::exists("something/this/way/comes"));
    EXPECT_FALSE(fs::exists("something/this/way"));

    EXPECT_TRUE(fs::exists("something/this/"));
    EXPECT_TRUE(fs::exists("something/this/somedir"));

    fs::remove_all("something");
}

TEST(PathTest, MD5FileExists)
{
    auto ofs = std::ofstream("md5.txt");
    ASSERT_TRUE(ofs);

    ofs << "This is a test.";
    ofs.close();

    EXPECT_EQ("12ea8a25e5d487bf68b5f709644019", path::MD5("md5.txt"));

    std::filesystem::remove("md5.txt");
}

TEST(PathTest, MD5FileNotExists)
{
    EXPECT_TRUE(path::MD5("idontexist.txt").empty());
}
}  // namespace smam
