#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "../src/utils/file.hpp"
#include "../src/utils/misc.h"
#include "../src/utils/smfs.h"
#include "../src/utils/version.h"

using namespace testing;

TEST(UtilsTest, FileConstruction)
{
    File f1("some/dir/;filename.txt");
    EXPECT_EQ("some/dir/", f1.path);
    EXPECT_EQ("filename.txt", f1.name);
    EXPECT_EQ(9, f1.at);

    File f2("some/dir/;");
    EXPECT_TRUE(f2.path.empty());
    EXPECT_TRUE(f2.name.empty());
    EXPECT_EQ(std::string::npos, f2.at);

    File f3(";/filename.txt");
    EXPECT_TRUE(f3.path.empty());
    EXPECT_TRUE(f3.name.empty());
    EXPECT_EQ(std::string::npos, f3.at);

    File f4("some/dir/filename.txt");
    EXPECT_TRUE(f4.path.empty());
    EXPECT_TRUE(f4.name.empty());
    EXPECT_EQ(std::string::npos, f4.at);
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

// LoadData follows same testing procedure as GetFiles

TEST(UtilsTest, WriteData)
{
    EXPECT_TRUE(SMFS::writeData());

    auto last = SMFS::fs::current_path();
    SMFS::fs::current_path("/");

    EXPECT_FALSE(SMFS::writeData());

    SMFS::fs::current_path(last);
}

TEST(UtilsTest, DataCacheFile)
{
    SMFS::addFile("file1", "addon1");  // add file, installing addon1

    ASSERT_TRUE(SMFS::isInstalled("addon1"));  // should be installed

    ASSERT_EQ(1, SMFS::getFiles("addon1").size());
    EXPECT_EQ("file1", *SMFS::getFiles("addon1").begin());

    EXPECT_FALSE(SMFS::eraseFile("file1", "addon2"));  // wrong addon
    EXPECT_TRUE(SMFS::eraseFile("file1", "addon1"));   // correct addon

    EXPECT_EQ(0, SMFS::getFiles("addon1").size());  // no files left ->
    EXPECT_FALSE(SMFS::isInstalled("addon1"));      // is not installed
}

TEST(UtilsTest, DataCacheAddon)
{
    SMFS::addFile("file1", "addon1");

    ASSERT_TRUE(SMFS::isInstalled("addon1"));  // should be installed

    ASSERT_EQ(1, SMFS::getFiles("addon1").size());
    EXPECT_EQ("file1", *SMFS::getFiles("addon1").begin());

    SMFS::eraseAddon("addon1");  // erase with its set of files

    ASSERT_EQ(0, SMFS::getFiles("addon1").size());  // empty file set
    EXPECT_FALSE(SMFS::isInstalled("addon1"));      // is not installed
}

// RemoveFile is checked by RemoveEmptyDirs + CountSharedFiles

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

TEST(UtilsTest, IsInstalled)
{
    SMFS::fs::path dataFile = ".smamdata";  // must be .smamdata
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

    ASSERT_TRUE(SMFS::loadData());

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

    fs::path      dataFile = ".smamdata";  // must be .smamdata
    std::ofstream ofs(dataFile, std::ios::trunc);
    EXPECT_TRUE(ofs);

    ofs << "addon1 plugins/bin1.smx\n";
    ofs << "addon1 gamedata/gd.txt\n";

    ofs << "addon2 plugins/bin2.smx\n";
    ofs << "addon2 gamedata/gd.txt\n";
    ofs << "addon2 translations/phrases.txt\n";

    ofs.close();

    ASSERT_TRUE(SMFS::loadData());

    EXPECT_THAT(
        SMFS::getFiles("addon1"),
        UnorderedElementsAre("plugins/bin1.smx", "gamedata/gd.txt"));

    EXPECT_THAT(
        SMFS::getFiles("addon2"),
        UnorderedElementsAre("plugins/bin2.smx", "gamedata/gd.txt",
                             "translations/phrases.txt"));

    ASSERT_TRUE(fs::remove(dataFile));
}

TEST(UtilsTest, CountSharedFiles)
{
    namespace fs = SMFS::fs;

    fs::path      dataFile = ".smamdata";  // must be .smamdata
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

    ASSERT_TRUE(SMFS::loadData());

    EXPECT_EQ(SMFS::countSharedFiles("plugins/bin1.smx"), 1);
    EXPECT_EQ(SMFS::countSharedFiles("plugins/bin2.smx"), 1);
    EXPECT_EQ(SMFS::countSharedFiles("plugins/bin3.smx"), 1);

    EXPECT_EQ(SMFS::countSharedFiles("gamedata/gd.txt"), 3);
    EXPECT_EQ(SMFS::countSharedFiles("translations/addon2.phrases.txt"),
              2);

    ASSERT_TRUE(fs::remove(dataFile));
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
