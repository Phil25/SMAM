#include <fstream>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../src/smfs.h"

#include <nlohmann/json.hpp>

using namespace testing;
using json   = nlohmann::json;
namespace fs = SMFS::fs;

constexpr const char* dataFile = ".smamdata";

namespace
{
auto addon(const std::string&           author      = "",
           const std::string&           description = "",
           const std::set<fs::path>&    files       = {},
           const std::set<std::string>& deps        = {})
{
    json addon;

    addon["author"]      = author;
    addon["description"] = description;
    addon["explicit"]    = true;

    if (files.size())
    {
        addon["files"] = files;
    }
    else
    {
        addon["files"] = json::array();
    }

    if (deps.size())
    {
        addon["deps"] = deps;
    }
    else
    {
        addon["deps"] = json::array();
    }

    return addon;
}

auto make(const json& data)
{
    json j;
    j["data"] = data;
    j["hash"] = std::hash<json>{}(j["data"]);
    return j;
}
}  // namespace

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

    auto last = fs::current_path();
    fs::current_path("/");

    EXPECT_FALSE(SMFS::Data::save());

    fs::current_path(last);
}

TEST(SMFSTest, DataCacheFile)
{
    SMFS::File::add("file1", "addon1");  // add file, installing addon1
    ASSERT_TRUE(SMFS::Addon::isInstalled("addon1"));

    ASSERT_EQ(1, SMFS::Addon::files("addon1").size());
    EXPECT_EQ("file1", *SMFS::Addon::files("addon1").begin());

    EXPECT_EQ(SMFS::DeleteResult::OK, SMFS::File::remove("file1"));

    EXPECT_EQ(0, SMFS::Addon::files("addon1").size());  // addon empty,
    EXPECT_TRUE(SMFS::Addon::isInstalled("addon1"));    // but installed
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

// File::remove is checked by Path::removeEmpty + File::find

TEST(SMFSTest, FileFind)
{
    std::ofstream ofs(dataFile, std::ios::trunc);
    ASSERT_TRUE(ofs);

    // clang-format off
    ofs << make({
        {"addon1", addon("", "", {
            "plugins/bin1.smx",
            "gamedata/gd.txt",
            "translations/phrases.txt",
        })},
        {"addon2", addon("", "", {
            "plugins/bin2.smx",
            "gamedata/gd.txt",
            "translations/phrases.txt",
        })},
        {"addon3", addon("", "", {
            "plugins/bin3.smx",
            "gamedata/gd.txt",
        })},
    });
    // clang-format on

    ofs.close();

    ASSERT_EQ(SMFS::LoadResult::OK, SMFS::Data::load());
    ASSERT_TRUE(fs::remove(dataFile));

    using namespace SMFS::File;
    EXPECT_THAT(find("plugins/bin1.smx"), Pair("addon1", 1));
    EXPECT_THAT(find("plugins/bin2.smx"), Pair("addon2", 1));
    EXPECT_THAT(find("plugins/bin3.smx"), Pair("addon3", 1));
    EXPECT_THAT(find("gamedata/gd.txt"), Pair("addon3", 3));
    EXPECT_THAT(find("translations/phrases.txt"), Pair("addon2", 2));
}

TEST(SMFSTest, AddonFiles)
{
    std::ofstream ofs(dataFile, std::ios::trunc);
    ASSERT_TRUE(ofs);

    // clang-format off
    ofs << make({
        {"addon1", addon("", "", {
            "plugins/bin1.smx",
            "gamedata/gd.txt",
        })},
        {"addon2", addon("", "", {
            "plugins/bin2.smx",
            "gamedata/gd.txt",
            "translations/phrases.txt",
        })},
    });
    // clang-format on

    ofs.close();

    ASSERT_EQ(SMFS::LoadResult::OK, SMFS::Data::load());
    ASSERT_TRUE(fs::remove(dataFile));

    std::set<fs::path> f1, f2;

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

    // clang-format off
    ofs << make({
        {"multifile", addon("", "", {
            "path/to/file1",
            "path/to/file2",
            "path/to/file3",
        })},
        {"spacemultifile", addon("", "", {
            "path/to/file1 with spaces1",
            "path/to/file1 with spaces2",
            "path/to/file1 with spaces3",
        })},
        {"singlefile", addon("", "", {
            "path/to/file4",
        })},
        {"spacesinglefile", addon("", "", {
            "path/to/file with spaces4",
        })},
        {"nofiles", addon()},
    });
    // clang-format on

    ofs.close();

    ASSERT_EQ(SMFS::LoadResult::OK, SMFS::Data::load());
    ASSERT_TRUE(fs::remove(dataFile));

    EXPECT_TRUE(SMFS::Addon::isInstalled("multifile"));
    EXPECT_TRUE(SMFS::Addon::isInstalled("spacemultifile"));

    EXPECT_TRUE(SMFS::Addon::isInstalled("singlefile"));
    EXPECT_TRUE(SMFS::Addon::isInstalled("spacesinglefile"));

    EXPECT_TRUE(SMFS::Addon::isInstalled("nofiles"));

    EXPECT_FALSE(SMFS::Addon::isInstalled("nothingatall"));
}
