#include <utils/common.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace testing;

namespace smam
{
TEST(UtilsTest, SplitLines)
{
    EXPECT_THAT(utils::SplitLines("Line1\nLine2\nLine3"),
                ElementsAre("Line1", "Line2", "Line3"));

    EXPECT_THAT(utils::SplitLines("Line1\\nLine1\nLine2"),
                ElementsAre("Line1\\nLine1", "Line2"));

    EXPECT_THAT(utils::SplitLines("Line1\\\nLine2\nLine3"),
                ElementsAre("Line1\\", "Line2", "Line3"));
}

TEST(UtilsTest, IsLink)
{
    EXPECT_TRUE(utils::IsLink("https://sth"));
    EXPECT_TRUE(utils::IsLink("http://sth"));

    EXPECT_FALSE(utils::IsLink("https:/sth"));
    EXPECT_FALSE(utils::IsLink("http:/sth"));

    EXPECT_FALSE(utils::IsLink("ttps://sth"));
    EXPECT_FALSE(utils::IsLink("ttp://sth"));

    EXPECT_FALSE(utils::IsLink(" https://sth"));
    EXPECT_FALSE(utils::IsLink(" http://sth"));

    EXPECT_FALSE(utils::IsLink("ahttps://sth"));
    EXPECT_FALSE(utils::IsLink("ahttp://sth"));
}

TEST(UtilsTest, ExtractString)
{
    EXPECT_EQ("value",
              utils::ExtractString("abcvaluedef", "abc", "def"));
    EXPECT_EQ("comment",
              utils::ExtractString("/*comment*/", "/*", "*/"));
    EXPECT_EQ("\nln2\n",
              utils::ExtractString("ln1\nln2\nln3", "ln1", "ln3"));
    EXPECT_EQ("No extr", utils::ExtractString("No extr", "", ""));
    EXPECT_EQ("No extr", utils::ExtractString("No extr", "No", ""));
    EXPECT_EQ("No extr", utils::ExtractString("No extr", "", "extr"));
}

TEST(UtilsTest, BiggestVersion)
{
    EXPECT_EQ("3.1", utils::BiggestVersion({"3.1"}));

    EXPECT_EQ("2.14", utils::BiggestVersion({"2.5", "2.14", "2.1"}));

    EXPECT_EQ("2.5.6-104", utils::BiggestVersion(
                               {"2.5.6-99", "2.5.6-104", "2.5.6-20",
                                "2.1-200", "2.5.6-103"}));
}

TEST(UtilsTest, BiggestVersionWithName)
{
    auto biggest = utils::BiggestVersion({
        "name_2.51-0.zip",
        "name_2.51-1.zip",
        "name_1.52.zip",
        "name_1.99-1.zip",
    });

    EXPECT_EQ("name_2.51-1.zip", biggest);
}
}  // namespace smam
