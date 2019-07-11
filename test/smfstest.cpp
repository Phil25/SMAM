#include <fstream>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../src/smfs.h"

constexpr const char* dataFile = ".smamdata";

TEST(SMFSTest, PathIsSafe)
{
    EXPECT_TRUE(SMFS::Path::isSafe(""));
    EXPECT_TRUE(SMFS::Path::isSafe("."));
    EXPECT_TRUE(SMFS::Path::isSafe("./"));
    EXPECT_TRUE(SMFS::Path::isSafe(".."));
    EXPECT_TRUE(SMFS::Path::isSafe("../"));
    EXPECT_TRUE(SMFS::Path::isSafe("../."));
    EXPECT_TRUE(SMFS::Path::isSafe(".././"));
    EXPECT_TRUE(SMFS::Path::isSafe("../.."));
    EXPECT_TRUE(SMFS::Path::isSafe("../../"));
    EXPECT_TRUE(SMFS::Path::isSafe("../../."));
    EXPECT_TRUE(SMFS::Path::isSafe("../.././"));

    EXPECT_FALSE(SMFS::Path::isSafe("../../.."));
    EXPECT_FALSE(SMFS::Path::isSafe("../../../"));
    EXPECT_FALSE(SMFS::Path::isSafe("../../../."));
    EXPECT_FALSE(SMFS::Path::isSafe("../../.././"));
    EXPECT_FALSE(SMFS::Path::isSafe("../../../.."));

    EXPECT_FALSE(SMFS::Path::isSafe(".././../.."));
    EXPECT_FALSE(SMFS::Path::isSafe("../.././../"));
    EXPECT_FALSE(SMFS::Path::isSafe(".././.././../."));
    EXPECT_FALSE(SMFS::Path::isSafe(".././../../././"));
    EXPECT_FALSE(SMFS::Path::isSafe("../../.././././.."));
}

TEST(SMFSTest, PathFindRoot)
{
    namespace fs = SMFS::fs;
    ASSERT_TRUE(
        fs::create_directories("./mod/addons/sourcemod/plugins"));

    auto equal = [](const fs::path& p) {
        static const fs::path root = "./mod/addons/sourcemod";

        auto mp = SMFS::Path::findRoot(p);
        return mp.has_value() && fs::equivalent(mp.value(), root);
    };

    EXPECT_FALSE(SMFS::Path::findRoot({}));
    EXPECT_FALSE(equal(""));
    EXPECT_FALSE(equal("."));
    EXPECT_FALSE(equal("./"));

    EXPECT_TRUE(equal("./mod"));
    EXPECT_TRUE(equal("./mod/addons"));
    EXPECT_TRUE(equal("./mod/addons/sourcemod"));
    EXPECT_TRUE(equal("./mod/addons/sourcemod/plugins"));

    fs::remove_all("./mod/");
}

TEST(SMFSTest, PathRemoveEmpty)
{
    namespace fs = SMFS::fs;

    fs::create_directories("something/this/way/comes");
    fs::create_directories("something/this/somedir");
    SMFS::Path::removeEmpty("something/this/way/comes");

    EXPECT_FALSE(fs::exists("something/this/way/comes"));
    EXPECT_FALSE(fs::exists("something/this/way"));

    EXPECT_TRUE(fs::exists("something/this/"));
    EXPECT_TRUE(fs::exists("something/this/somedir"));

    fs::remove_all("something");
}

// Data::load follows same testing procedure as Addon::files

TEST(SMFSTest, DataSave)
{
    EXPECT_TRUE(SMFS::Data::save());

    auto last = SMFS::fs::current_path();
    SMFS::fs::current_path("/");

    EXPECT_FALSE(SMFS::Data::save());

    SMFS::fs::current_path(last);
}

TEST(SMFSTest, DataCacheFile)
{
    SMFS::File::add("file1", "addon1");  // add file, installing addon1
    ASSERT_TRUE(SMFS::Addon::isInstalled("addon1"));

    ASSERT_EQ(1, SMFS::Addon::files("addon1").size());
    EXPECT_EQ("file1", *SMFS::Addon::files("addon1").begin());

    EXPECT_FALSE(SMFS::File::detach("file1", "addon2"));  // wrong addon
    EXPECT_TRUE(SMFS::File::detach("file1", "addon1"));

    EXPECT_EQ(0, SMFS::Addon::files("addon1").size());  // empty addon->
    EXPECT_FALSE(SMFS::Addon::isInstalled("addon1"));   // not installed
}

TEST(SMFSTest, DataCacheAddon)
{
    SMFS::File::add("file1", "addon1");  // add file, installing addon1
    ASSERT_TRUE(SMFS::Addon::isInstalled("addon1"));

    ASSERT_EQ(1, SMFS::Addon::files("addon1").size());
    EXPECT_EQ("file1", *SMFS::Addon::files("addon1").begin());

    SMFS::Addon::erase("addon1");  // erase with its set of files

    ASSERT_EQ(0, SMFS::Addon::files("addon1").size());  // empty addon->
    EXPECT_FALSE(SMFS::Addon::isInstalled("addon1"));   // not installed
}

// File::remove is checked by Path::removeEmpty + File::countShared

TEST(SMFSTest, FileCountShared)
{
    std::ofstream ofs(dataFile, std::ios::trunc);
    ASSERT_TRUE(ofs);

    ofs << "addon1 plugins/bin1.smx\n";
    ofs << "addon1 gamedata/gd.txt\n";  // shared with rest

    ofs << "addon2 plugins/bin2.smx\n";
    ofs << "addon2 gamedata/gd.txt\n";  // shared with rest
    ofs << "addon2 translations/addon2.phrases.txt\n";  // shared with 3

    ofs << "addon3 plugins/bin3.smx\n";
    ofs << "addon3 gamedata/gd.txt\n";  // shared with rest
    ofs << "addon3 translations/addon2.phrases.txt";  // shared with 2

    ofs.close();

    ASSERT_TRUE(SMFS::Data::load());
    ASSERT_TRUE(SMFS::fs::remove(dataFile));

    EXPECT_EQ(SMFS::File::countShared("plugins/bin1.smx"), 1);
    EXPECT_EQ(SMFS::File::countShared("plugins/bin2.smx"), 1);
    EXPECT_EQ(SMFS::File::countShared("plugins/bin3.smx"), 1);

    EXPECT_EQ(SMFS::File::countShared("gamedata/gd.txt"), 3);
    EXPECT_EQ(
        SMFS::File::countShared("translations/addon2.phrases.txt"), 2);
}

TEST(SMFSTest, AddonFiles)
{
    std::ofstream ofs(dataFile, std::ios::trunc);
    ASSERT_TRUE(ofs);

    ofs << "addon1 plugins/bin1.smx\n";
    ofs << "addon1 gamedata/gd.txt\n";

    ofs << "addon2 plugins/bin2.smx\n";
    ofs << "addon2 gamedata/gd.txt\n";
    ofs << "addon2 translations/phrases.txt";

    ofs.close();

    ASSERT_TRUE(SMFS::Data::load());
    ASSERT_TRUE(SMFS::fs::remove(dataFile));

    std::set<SMFS::fs::path> f1, f2;

    f1 = {"plugins/bin1.smx", "gamedata/gd.txt"};
    EXPECT_EQ(f1, SMFS::Addon::files("addon1"));

    f2 = {"plugins/bin2.smx", "gamedata/gd.txt",
          "translations/phrases.txt"};
    EXPECT_EQ(f2, SMFS::Addon::files("addon2"));
}

TEST(SMFSTest, AddonIsInstalled)
{
    std::ofstream ofs(dataFile, std::ios::trunc);
    ASSERT_TRUE(ofs);

    ofs << "multifile path/to/file1\n";
    ofs << "multifile path/to/file2\n";
    ofs << "multifile path/to/file3\n";

    ofs << "spacemultifile \"path/to/file with spaces1\"\n";
    ofs << "spacemultifile \"path/to/file with spaces2\"\n";
    ofs << "spacemultifile \"path/to/file with spaces3\"\n";

    ofs << "singlefile path/to/file4\n";
    ofs << "spacesinglefile \"path/to/file with spaces4\"\n";

    ofs << "nofiles";

    ofs.close();

    ASSERT_TRUE(SMFS::Data::load());
    ASSERT_TRUE(SMFS::fs::remove(dataFile));

    EXPECT_TRUE(SMFS::Addon::isInstalled("multifile"));
    EXPECT_TRUE(SMFS::Addon::isInstalled("spacemultifile"));

    EXPECT_TRUE(SMFS::Addon::isInstalled("singlefile"));
    EXPECT_TRUE(SMFS::Addon::isInstalled("spacesinglefile"));

    EXPECT_FALSE(SMFS::Addon::isInstalled("nofiles"));
    EXPECT_FALSE(SMFS::Addon::isInstalled("nothingatall"));
}
