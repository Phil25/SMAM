#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "../src/utils/file.hpp"
#include "../src/utils/misc.h"
#include "../src/utils/smfs.h"
#include "../src/utils/version.h"
#include "gtest/gtest.h"

TEST(UtilsTest, FileConstr)
{
    File f("some/dir/;file;name.txt");
    EXPECT_EQ("some/dir/", f.path);
    EXPECT_EQ("file;name.txt", f.name);
}

TEST(UtilsTest, ToLines)
{
    std::vector<std::string> vec =
        Utils::toLines("Line1\nLine2\nLine3");
    ASSERT_EQ(3, vec.size());

    EXPECT_EQ("Line1", vec.at(0));
    EXPECT_EQ("Line2", vec.at(1));
    EXPECT_EQ("Line3", vec.at(2));
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
    std::string value = Utils::extract("abcvaluedef", "abc", "def");
    EXPECT_EQ("value", value);

    std::string comment = Utils::extract("/*comment*/", "/*", "*/");
    EXPECT_EQ("comment", comment);

    std::string ln2 = Utils::extract("ln1\nln2\nln3", "ln1", "ln3");
    EXPECT_EQ("\nln2\n", ln2);

    std::string noextract1 = Utils::extract("No extract", "", "");
    EXPECT_EQ("No extract", noextract1);

    std::string noextract2 = Utils::extract("No extract", "No", "");
    EXPECT_EQ("No extract", noextract2);

    std::string noextract3 =
        Utils::extract("No extract", "", "extract");
    EXPECT_EQ("No extract", noextract3);
}

TEST(UtilsTest, FindRoot)
{
    namespace fs = SMFS::fs;
    ASSERT_TRUE(
        fs::create_directories("./mod/addons/sourcemod/plugins"));

    auto equal = [](const fs::path& p) {
        static const fs::path root = "./mod/addons/sourcemod";

        auto mp = SMFS::findRoot(p);
        return mp.has_value() && fs::equivalent(mp.value(), root);
    };

    EXPECT_FALSE(SMFS::findRoot({}));
    EXPECT_FALSE(equal(""));
    EXPECT_FALSE(equal("."));
    EXPECT_FALSE(equal("./"));

    EXPECT_TRUE(equal("./mod"));
    EXPECT_TRUE(equal("./mod/addons"));
    EXPECT_TRUE(equal("./mod/addons/sourcemod"));
    EXPECT_TRUE(equal("./mod/addons/sourcemod/plugins"));

    fs::remove_all("./mod/");
}

TEST(UtilsTest, IsPathSafe)
{
    EXPECT_TRUE(SMFS::isPathSafe(""));
    EXPECT_TRUE(SMFS::isPathSafe("."));
    EXPECT_TRUE(SMFS::isPathSafe("./"));
    EXPECT_TRUE(SMFS::isPathSafe(".."));
    EXPECT_TRUE(SMFS::isPathSafe("../"));
    EXPECT_TRUE(SMFS::isPathSafe("../."));
    EXPECT_TRUE(SMFS::isPathSafe(".././"));
    EXPECT_TRUE(SMFS::isPathSafe("../.."));
    EXPECT_TRUE(SMFS::isPathSafe("../../"));
    EXPECT_TRUE(SMFS::isPathSafe("../../."));
    EXPECT_TRUE(SMFS::isPathSafe("../.././"));

    EXPECT_FALSE(SMFS::isPathSafe("../../.."));
    EXPECT_FALSE(SMFS::isPathSafe("../../../"));
    EXPECT_FALSE(SMFS::isPathSafe("../../../."));
    EXPECT_FALSE(SMFS::isPathSafe("../../.././"));
    EXPECT_FALSE(SMFS::isPathSafe("../../../.."));

    EXPECT_FALSE(SMFS::isPathSafe(".././../.."));
    EXPECT_FALSE(SMFS::isPathSafe("../.././../"));
    EXPECT_FALSE(SMFS::isPathSafe(".././.././../."));
    EXPECT_FALSE(SMFS::isPathSafe(".././../../././"));
    EXPECT_FALSE(SMFS::isPathSafe("../../.././././.."));
}

TEST(UtilsTest, RemoveEmptyDirs)
{
    namespace fs = SMFS::fs;

    fs::create_directories("something/this/way/comes");
    fs::create_directories("something/this/somedir");
    SMFS::removeEmptyDirs("something/this/way/comes");

    EXPECT_FALSE(fs::exists("something/this/way/comes"));
    EXPECT_FALSE(fs::exists("something/this/way"));

    EXPECT_TRUE(fs::exists("something/this/"));
    EXPECT_TRUE(fs::exists("something/this/somedir"));

    fs::remove_all("something");
}

TEST(UtilsTest, WriteData)
{
    EXPECT_TRUE(SMFS::writeData());

    auto last = SMFS::fs::current_path();
    SMFS::fs::current_path("/");

    EXPECT_FALSE(SMFS::writeData());

    SMFS::fs::current_path(last);
}

TEST(UtilsTest, IsInstalled)
{
    SMFS::fs::path dataFile = "testsmamdata";
    std::ofstream  ofs(dataFile, std::ios::trunc);
    ASSERT_TRUE(ofs);

    ofs << "multifile path/to/file1\n";
    ofs << "multifile path/to/file2\n";
    ofs << "multifile path/to/file3\n";

    ofs << "spacemultifile \"path/to/file with spaces1\"\n";
    ofs << "spacemultifile \"path/to/file with spaces2\"\n";
    ofs << "spacemultifile \"path/to/file with spaces3\"\n";

    ofs << "singlefile path/to/file4\n";
    ofs << "spacesinglefile \"path/to/file with spaces4\"\n";

    ofs << "nofiles\n";

    ofs.close();

    SMFS::loadData(dataFile);

    EXPECT_TRUE(SMFS::isInstalled("multifile"));
    EXPECT_TRUE(SMFS::isInstalled("spacemultifile"));

    EXPECT_TRUE(SMFS::isInstalled("singlefile"));
    EXPECT_TRUE(SMFS::isInstalled("spacesinglefile"));

    EXPECT_FALSE(SMFS::isInstalled("nofiles"));
    EXPECT_FALSE(SMFS::isInstalled("nothingatall"));

    ASSERT_TRUE(SMFS::fs::remove(dataFile));
}

TEST(UtilsTest, GetFiles)
{
    namespace fs = SMFS::fs;

    fs::path      dataFile = "testsmamdata";
    std::ofstream ofs(dataFile, std::ios::trunc);
    EXPECT_TRUE(ofs);

    ofs << "addon1 plugins/bin1.smx\n";
    ofs << "addon1 gamedata/gd.txt\n";

    ofs << "addon2 plugins/bin2.smx\n";
    ofs << "addon2 gamedata/gd.txt\n";
    ofs << "addon2 translations/phrases.txt\n";

    ofs.close();

    SMFS::loadData(dataFile);

    auto comp = [](std::set<fs::path> expected,
                   std::set<fs::path> actual) {
        ASSERT_EQ(expected.size(), actual.size());

        auto end = expected.end();
        auto exp = expected.begin();
        auto act = actual.begin();

        for (; exp != end; ++exp, ++act)
        {
            EXPECT_EQ(*exp, *act);
        }
    };

    comp({"plugins/bin1.smx", "gamedata/gd.txt"},
         SMFS::getFiles("addon1"));
    comp({"plugins/bin2.smx", "gamedata/gd.txt",
          "translations/phrases.txt"},
         SMFS::getFiles("addon2"));

    fs::remove(dataFile);
}

TEST(UtilsTest, CountSharedFiles)
{
    namespace fs = SMFS::fs;

    fs::path      dataFile = "testsmamdata";
    std::ofstream ofs(dataFile, std::ios::trunc);
    EXPECT_TRUE(ofs);

    ofs << "addon1 plugins/bin1.smx\n";
    ofs << "addon1 gamedata/gd.txt\n";  // shared with rest

    ofs << "addon2 plugins/bin2.smx\n";
    ofs << "addon2 gamedata/gd.txt\n";  // shared with rest
    ofs << "addon2 translations/addon2.phrases.txt\n";  // shared with 3

    ofs << "addon3 plugins/bin3.smx\n";
    ofs << "addon3 gamedata/gd.txt\n";  // shared with rest
    ofs << "addon3 translations/addon2.phrases.txt\n";  // shared with 2

    ofs.close();

    SMFS::loadData(dataFile);

    EXPECT_EQ(SMFS::countSharedFiles("plugins/bin1.smx"), 1);
    EXPECT_EQ(SMFS::countSharedFiles("plugins/bin2.smx"), 1);
    EXPECT_EQ(SMFS::countSharedFiles("plugins/bin3.smx"), 1);

    EXPECT_EQ(SMFS::countSharedFiles("gamedata/gd.txt"), 3);
    EXPECT_EQ(SMFS::countSharedFiles("translations/addon2.phrases.txt"),
              2);

    fs::remove(dataFile);
}

TEST(UtilsTest, VersionBiggest)
{
    std::vector<std::string> vers0 = {"3.1"};
    EXPECT_EQ("3.1", Utils::Version::biggest(vers0));

    std::vector<std::string> vers = {
        "2.5",
        "2.14",
        "2.1",
    };
    EXPECT_EQ("2.14", Utils::Version::biggest(vers));

    std::vector<std::string> vers2 = {
        "2.5.6-99", "2.5.6-104", "2.5.6-20", "2.1-200", "2.5.6-103"};
    EXPECT_EQ("2.5.6-104", Utils::Version::biggest(vers2));
}

TEST(UtilsTest, VersionBiggestName)
{
    std::vector<std::string> versions = {
        "name_2.51-0.zip",
        "name_2.51-1.zip",
        "name_1.52.zip",
        "name_1.99-1.zip",
    };
    EXPECT_EQ("name_2.51-1.zip", Utils::Version::biggest(versions));
}
