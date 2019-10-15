#include <data/addon.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <fstream>
#include <nlohmann/json.hpp>

namespace smam
{
using namespace testing;
constexpr const char* dataFile = ".smamdata.json";

class AddonTest : public ::testing::Test
{
protected:
    void TearDown() override
    {
        Addon::ForEach([](const auto& addon) { addon->Erase(); });
    }
};

class AddonTestPreinstalled : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // clang-format off
        nlohmann::json{
            {"id", "test"},
            {"author", "Somedev"},
            {"description", "Test addon"},
            {"explicit", true},
            {"files", {"plugins/bin.smx", "gamedata/data.txt"}},
            {"deps", {"plugin1", "test2"}},
        }.get<AddonPtr>()->MarkInstalled();

        nlohmann::json{
            {"id", "test2"},
            {"author", "Somedev"},
            {"description", "Test addon2"},
            {"explicit", false},
            {"files", {"plugins/bin.smx", "translations/tr.txt"}},
            {"deps", {"plugin2", "plugin3"}},
        }.get<AddonPtr>()->MarkInstalled();
        // clang-format on
    }

    void TearDown() override
    {
        Addon::ForEach([](const auto& addon) { addon->Erase(); });
    }
};

inline auto get(std::string file)
{
    return nlohmann::json(std::move(file)).get<FilePtr>();
}

TEST_F(AddonTest, DeserializeFullLocal)
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

TEST_F(AddonTest, DeserializePartialLocal)
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

TEST_F(AddonTest, DeserializeFullRemote)
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

TEST_F(AddonTest, DeserializePartialRemote)
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

TEST_F(AddonTest, SerializeFull)
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

TEST_F(AddonTest, SerializePartial)
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

TEST_F(AddonTest, AddFiles)
{
    auto plugin   = get("plugins/bin.smx");
    auto gamedata = get("gamedata/data.txt");
    auto source   = get("scripting/bin.sp");

    auto addon = Addon("test", {}, {});
    ASSERT_TRUE(addon.Files().empty());

    addon.AddFiles({plugin});
    ASSERT_EQ(1, addon.Files().size());
    EXPECT_THAT(addon.Files(), ElementsAre(plugin));

    addon.AddFiles({gamedata, source});
    ASSERT_EQ(3, addon.Files().size());
    EXPECT_THAT(addon.Files(), ElementsAre(plugin, gamedata, source));
}

TEST_F(AddonTestPreinstalled, CountByOwnedFile)
{
    EXPECT_EQ(2, Addon::CountByOwnedFile(get("plugins/bin.smx")));
    EXPECT_EQ(1, Addon::CountByOwnedFile(get("gamedata/data.txt")));
    EXPECT_EQ(1, Addon::CountByOwnedFile(get("translations/tr.txt")));
}

TEST_F(AddonTestPreinstalled, Save)
{
    ASSERT_TRUE(Addon::Save(dataFile));

    auto ifs = std::ifstream(dataFile);
    ASSERT_TRUE(ifs);

    auto json = nlohmann::json::parse(ifs);
    ifs.close();

    ASSERT_TRUE(json.count("data"));
    ASSERT_TRUE(json.count("hash"));

    ASSERT_EQ(7755100675613232546u, json.at("hash").get<size_t>());
    ASSERT_EQ(2, json.at("data").size());

    const auto& a1 = json["data"][0];
    const auto& a2 = json["data"][1];

    EXPECT_EQ("test", a1["id"]);
    EXPECT_EQ("Somedev", a1["author"]);
    EXPECT_EQ("Test addon", a1["description"]);
    EXPECT_TRUE(a1["explicit"]);

    EXPECT_THAT(a1["files"].get<std::vector<std::string>>(),
                ElementsAre("plugins/bin.smx", "gamedata/data.txt"));

    EXPECT_THAT(a1["deps"].get<std::vector<std::string>>(),
                ElementsAre("plugin1", "test2"));

    EXPECT_EQ("test2", a2["id"]);
    EXPECT_EQ("Somedev", a2["author"]);
    EXPECT_EQ("Test addon2", a2["description"]);
    EXPECT_FALSE(a2["explicit"]);

    EXPECT_THAT(a2["files"].get<std::vector<std::string>>(),
                ElementsAre("plugins/bin.smx", "translations/tr.txt"));

    EXPECT_THAT(a2["deps"].get<std::vector<std::string>>(),
                ElementsAre("plugin2", "plugin3"));
}

TEST_F(AddonTest, LoadCreate)
{
    EXPECT_TRUE(Addon::Load(".smamdata.json"));
}

TEST_F(AddonTest, Load)
{
    auto ofs = std::ofstream(dataFile, std::ios::trunc);
    ASSERT_TRUE(ofs);

    // clang-format off
    auto json = nlohmann::json{{"data", {
        {
            {"id", "test"},
            {"author", "Somedev"},
            {"description", "Test addon"},
            {"explicit", true},
            {"files", {"plugins/bin.smx", "gamedata/data.txt"}},
            {"deps", {"plugin1", "test2"}},
        },
        {
            {"id", "test2"},
            {"author", "Somedev"},
            {"description", "Test addon2"},
            {"explicit", false},
            {"files", {"plugins/bin2.smx", "translations/tr.txt"}},
            {"deps", {"plugin2", "plugin3"}},
        }
    }}, {"hash", 3810067853656611362u}};
    // clang-format on

    ofs << json;
    ofs.close();

    ASSERT_TRUE(Addon::Load(dataFile));

    try
    {
        auto test = Addon::Get("test").value();
        EXPECT_EQ("test", test->ID());
        EXPECT_EQ("Somedev", test->Author());
        EXPECT_EQ("Test addon", test->Description());
        EXPECT_TRUE(test->IsExplicit());
        EXPECT_EQ("plugins/bin.smx", test->Files().at(0)->Raw());
        EXPECT_EQ("gamedata/data.txt", test->Files().at(1)->Raw());
        EXPECT_THAT(test->Dependencies(),
                    ElementsAre("plugin1", "test2"));

        auto test2 = Addon::Get("test2").value();
        EXPECT_EQ("test2", test2->ID());
        EXPECT_EQ("Somedev", test2->Author());
        EXPECT_EQ("Test addon2", test2->Description());
        EXPECT_FALSE(test2->IsExplicit());
        EXPECT_EQ("plugins/bin2.smx", test2->Files().at(0)->Raw());
        EXPECT_EQ("translations/tr.txt", test2->Files().at(1)->Raw());
        EXPECT_THAT(test2->Dependencies(),
                    ElementsAre("plugin2", "plugin3"));
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}
}  // namespace smam
