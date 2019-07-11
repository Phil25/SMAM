#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../src/utils/file.hpp"
#include "../src/utils/misc.h"
#include "../src/utils/version.h"

using namespace testing;

TEST(UtilsTest, FileConstruction)
{
    File f1("some/dir/;filename.txt");
    EXPECT_EQ("some/dir/", f1.path);
    EXPECT_EQ("filename.txt", f1.name);
    EXPECT_TRUE(f1.valid());

    File f2("some/dir/;");
    EXPECT_TRUE(f2.path.empty());
    EXPECT_TRUE(f2.name.empty());
    EXPECT_FALSE(f2.valid());

    File f3(";/filename.txt");
    EXPECT_TRUE(f3.path.empty());
    EXPECT_TRUE(f3.name.empty());
    EXPECT_FALSE(f3.valid());

    File f4("some/dir/filename.txt");
    EXPECT_TRUE(f4.path.empty());
    EXPECT_TRUE(f4.name.empty());
    EXPECT_FALSE(f4.valid());
}

TEST(UtilsTest, ToLines)
{
    EXPECT_THAT(Utils::toLines("Line1\nLine2\nLine3"),
                ElementsAre("Line1", "Line2", "Line3"));

    EXPECT_THAT(Utils::toLines("Line1\\nLine1\nLine2"),
                ElementsAre("Line1\\nLine1", "Line2"));

    EXPECT_THAT(Utils::toLines("Line1\\\nLine2\nLine3"),
                ElementsAre("Line1\\", "Line2", "Line3"));
}

TEST(UtilsTest, IsLink)
{
    EXPECT_TRUE(Utils::isLink("https://sth"));
    EXPECT_TRUE(Utils::isLink("http://sth"));

    EXPECT_FALSE(Utils::isLink("https:/sth"));
    EXPECT_FALSE(Utils::isLink("http:/sth"));

    EXPECT_FALSE(Utils::isLink("ttps://sth"));
    EXPECT_FALSE(Utils::isLink("ttp://sth"));

    EXPECT_FALSE(Utils::isLink(" https://sth"));
    EXPECT_FALSE(Utils::isLink(" http://sth"));

    EXPECT_FALSE(Utils::isLink("ahttps://sth"));
    EXPECT_FALSE(Utils::isLink("ahttp://sth"));
}

TEST(UtilsTest, Extract)
{
    EXPECT_EQ("value", Utils::extract("abcvaluedef", "abc", "def"));
    EXPECT_EQ("comment", Utils::extract("/*comment*/", "/*", "*/"));
    EXPECT_EQ("\nln2\n", Utils::extract("ln1\nln2\nln3", "ln1", "ln3"));
    EXPECT_EQ("No extr", Utils::extract("No extr", "", ""));
    EXPECT_EQ("No extr", Utils::extract("No extr", "No", ""));
    EXPECT_EQ("No extr", Utils::extract("No extr", "", "extr"));
}

TEST(UtilsTest, VersionBiggest)
{
    EXPECT_EQ("3.1", Utils::Version::biggest({"3.1"}));

    EXPECT_EQ("2.14", Utils::Version::biggest({"2.5", "2.14", "2.1"}));

    EXPECT_EQ("2.5.6-104", Utils::Version::biggest(
                               {"2.5.6-99", "2.5.6-104", "2.5.6-20",
                                "2.1-200", "2.5.6-103"}));
}

TEST(UtilsTest, VersionBiggestName)
{
    EXPECT_EQ("name_2.51-1.zip", Utils::Version::biggest({
                                     "name_2.51-0.zip",
                                     "name_2.51-1.zip",
                                     "name_1.52.zip",
                                     "name_1.99-1.zip",
                                 }));
}
