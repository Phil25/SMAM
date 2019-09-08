#include "../../src/data/addon.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <nlohmann/json.hpp>

namespace smam
{
using namespace testing;
constexpr const char* dataFile = ".smamdata.json";

inline auto get(std::string file)
{
    return nlohmann::json(std::move(file)).get<FilePtr>();
}

TEST(AddonTest, DeserializeFullLocal)
{
    auto plugin   = get("plugins/bin.smx");
    auto gamedata = get("gamedata/data.txt");
    auto source   = get("scripting/bin.sp");

    // clang-format off
    auto addon = nlohmann::json{
            {"id", "test"},
            {"author", "Somedev"},
            {"description", "Test addon"},
            {"explicit", true},
            {"files", {plugin->Raw(), gamedata->Raw(), source->Raw()}},
            {"deps", {"someplugin", "someextension"}},
        }.get<AddonPtr>();
    // clang-format on

    EXPECT_EQ("test", addon->ID());
    EXPECT_EQ("Somedev", addon->Author());
    EXPECT_EQ("Test addon", addon->Description());
    EXPECT_TRUE(addon->IsExplicit());

    EXPECT_THAT(addon->Files(), ElementsAre(plugin, gamedata, source));

    EXPECT_THAT(addon->Dependencies(),
                UnorderedElementsAre("someplugin", "someextension"));
}

TEST(AddonTest, DeserializePartialLocal)
{
    // clang-format off
    auto addon = nlohmann::json{
            {"id", "test"},
            {"author", "Somedev"},
            {"description", "Test addon"},
        }.get<AddonPtr>();
    // clang-format on

    EXPECT_EQ("test", addon->ID());
    EXPECT_EQ("Somedev", addon->Author());
    EXPECT_EQ("Test addon", addon->Description());
    EXPECT_FALSE(addon->IsExplicit());

    EXPECT_TRUE(addon->Files().empty());
    EXPECT_TRUE(addon->Dependencies().empty());
}

TEST(AddonTest, DeserializeFullRemote)
{
    auto plugin   = get("plugins/;bin.smx");
    auto gamedata = get("gamedata/;data.txt");
    auto source   = get("scripting/;bin.sp");

    // clang-format off
    auto addon = nlohmann::json{
            {"id", "test"},
            {"author", "Somedev"},
            {"description", "Test addon"},
            {"explicit", true},
            {"files", {plugin->Raw(), gamedata->Raw(), source->Raw()}},
            {"deps", {"someplugin", "someextension"}},
        }.get<AddonPtr>();
    // clang-format on

    EXPECT_EQ("test", addon->ID());
    EXPECT_EQ("Somedev", addon->Author());
    EXPECT_EQ("Test addon", addon->Description());
    EXPECT_TRUE(addon->IsExplicit());

    EXPECT_THAT(addon->Files(), ElementsAre(plugin, gamedata, source));

    EXPECT_THAT(addon->Dependencies(),
                UnorderedElementsAre("someplugin", "someextension"));
}

TEST(AddonTest, DeserializePartialRemote)
{
    // clang-format off
    auto addon = nlohmann::json{
            {"id", "test"},
            {"author", "Somedev"},
            {"description", "Test addon"},
        }.get<AddonPtr>();
    // clang-format on

    EXPECT_EQ("test", addon->ID());
    EXPECT_EQ("Somedev", addon->Author());
    EXPECT_EQ("Test addon", addon->Description());
    EXPECT_FALSE(addon->IsExplicit());

    EXPECT_TRUE(addon->Files().empty());
    EXPECT_TRUE(addon->Dependencies().empty());
}

TEST(AddonTest, SerializeFull)
{
    auto plugin   = get("plugins/bin.smx");
    auto gamedata = get("gamedata/data.txt");
    auto source   = get("scripting/bin.sp");

    // clang-format off
    auto addon = nlohmann::json{
            {"id", "test"},
            {"author", "Somedev"},
            {"description", "Test addon"},
            {"explicit", true},
            {"files", {plugin->Raw(), gamedata->Raw(), source->Raw()}},
            {"deps", {"someplugin", "someextension"}},
        }.get<AddonPtr>();
    // clang-format on

    nlohmann::json json = addon;

    EXPECT_EQ("test", json["id"]);
    EXPECT_EQ("Somedev", json["author"]);
    EXPECT_EQ("Test addon", json["description"]);
    EXPECT_TRUE(json["explicit"]);

    std::vector<FilePtr> files;
    json.at("files").get_to(files);
    EXPECT_THAT(files, ElementsAre(plugin, gamedata, source));

    std::set<std::string> deps;
    json.at("deps").get_to(deps);
    EXPECT_THAT(deps,
                UnorderedElementsAre("someplugin", "someextension"));
}

TEST(AddonTest, SerializePartial)
{
    // clang-format off
    auto addon = nlohmann::json{
            {"id", "test"},
            {"author", "Somedev"},
            {"description", "Test addon"},
        }.get<AddonPtr>();
    // clang-format on

    nlohmann::json json = addon;

    EXPECT_EQ("test", json.at("id"));
    EXPECT_EQ("Somedev", json.at("author"));
    EXPECT_EQ("Test addon", json.at("description"));

    EXPECT_FALSE(json.at("explicit"));
    EXPECT_THAT(json.at("files"), IsEmpty());
    EXPECT_THAT(json.at("deps"), IsEmpty());
}
}  // namespace smam
