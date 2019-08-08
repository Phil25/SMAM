#include "../../src/smfs/path.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace testing;
namespace fs = std::filesystem;

TEST(PathTest, PathIsSafe)
{
    EXPECT_TRUE(Path::isSafe(""));
    EXPECT_TRUE(Path::isSafe("."));
    EXPECT_TRUE(Path::isSafe("./"));
    EXPECT_TRUE(Path::isSafe(".."));
    EXPECT_TRUE(Path::isSafe("../"));
    EXPECT_TRUE(Path::isSafe("../."));
    EXPECT_TRUE(Path::isSafe(".././"));
    EXPECT_TRUE(Path::isSafe("../.."));
    EXPECT_TRUE(Path::isSafe("../../"));
    EXPECT_TRUE(Path::isSafe("../../."));
    EXPECT_TRUE(Path::isSafe("../.././"));

    EXPECT_FALSE(Path::isSafe("../../.."));
    EXPECT_FALSE(Path::isSafe("../../../"));
    EXPECT_FALSE(Path::isSafe("../../../."));
    EXPECT_FALSE(Path::isSafe("../../.././"));
    EXPECT_FALSE(Path::isSafe("../../../.."));

    EXPECT_FALSE(Path::isSafe(".././../.."));
    EXPECT_FALSE(Path::isSafe("../.././../"));
    EXPECT_FALSE(Path::isSafe(".././.././../."));
    EXPECT_FALSE(Path::isSafe(".././../../././"));
    EXPECT_FALSE(Path::isSafe("../../.././././.."));
}

TEST(PathTest, PathFindRoot)
{
    fs::remove_all("mod");
    ASSERT_TRUE(fs::create_directories("mod/addons/sourcemod/plugins"));

    const fs::path root = "mod/addons/sourcemod";

    auto equal = [&root](const fs::path& startFrom) {
        auto mp = Path::findRoot(startFrom);
        return mp.has_value() && fs::equivalent(mp.value(), root);
    };

    EXPECT_FALSE(Path::findRoot({}));
    EXPECT_FALSE(equal(""));
    EXPECT_FALSE(equal("."));
    EXPECT_FALSE(equal("./"));

    EXPECT_TRUE(equal("./mod"));
    EXPECT_TRUE(equal("./mod/addons"));
    EXPECT_TRUE(equal("./mod/addons/sourcemod"));
    EXPECT_TRUE(equal("./mod/addons/sourcemod/plugins"));

    fs::create_directories("./mod/addons/sourcemod/plugins");
    fs::remove_all("mod");
}

TEST(PathTest, PathRemoveEmpty)
{
    fs::remove_all("something");
    fs::create_directories("something/this/way/comes");
    fs::create_directories("something/this/somedir");

    Path::removeEmpty("something/this/way/comes");

    EXPECT_FALSE(fs::exists("something/this/way/comes"));
    EXPECT_FALSE(fs::exists("something/this/way"));

    EXPECT_TRUE(fs::exists("something/this/"));
    EXPECT_TRUE(fs::exists("something/this/somedir"));

    fs::remove_all("something");
}
