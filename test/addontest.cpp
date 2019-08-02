#include "../src/smfs/addon.h"

#include <fstream>
#include <functional>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <nlohmann/json.hpp>

using namespace testing;
using json = nlohmann::json;

constexpr const char* dataFile = ".smamdata.json";

inline auto get(const std::string& file)
{
    return json(file).get<File::Ptr>();
}

inline auto getFiles(const std::shared_ptr<Addon>& addon) noexcept
{
    auto files = std::vector<File::Ptr>();

    addon->forEachFile(
        [&files](const auto& file) { files.push_back(file); });

    return files;
}

inline auto getDeps(const std::shared_ptr<Addon>& addon) noexcept
{
    auto deps = std::vector<std::string>();

    addon->forEachDep([&deps](const auto& dep) {
        deps.push_back(dep);
        return true;
    });

    return deps;
}

TEST(AddonTest, DeserializeFullLocal)
{
    auto plugin   = get("plugins/bin.smx");
    auto gamedata = get("gamedata/data.txt");
    auto source   = get("scripting/bin.sp");

    // clang-format off
    auto addon = json{
            {"id", "test"},
            {"author", "Somedev"},
            {"description", "Test addon"},
            {"explicit", true},
            {"files", {plugin->raw(), gamedata->raw(), source->raw()}},
            {"deps", {"someplugin", "someextension"}},
        }.get<std::shared_ptr<Addon>>();
    // clang-format on

    EXPECT_EQ("test", addon->getId());
    EXPECT_EQ("Somedev", addon->getAuthor());
    EXPECT_EQ("Test addon", addon->getDescription());
    EXPECT_TRUE(addon->isExplicit());

    EXPECT_THAT(getFiles(addon), ElementsAre(plugin, gamedata, source));

    EXPECT_THAT(getDeps(addon),
                UnorderedElementsAre("someplugin", "someextension"));
}

TEST(AddonTest, DeserializePartialLocal)
{
    // clang-format off
    auto addon = json{
            {"id", "test"},
            {"author", "Somedev"},
            {"description", "Test addon"},
        }.get<std::shared_ptr<Addon>>();
    // clang-format on

    EXPECT_EQ("test", addon->getId());
    EXPECT_EQ("Somedev", addon->getAuthor());
    EXPECT_EQ("Test addon", addon->getDescription());
    EXPECT_FALSE(addon->isExplicit());

    EXPECT_THAT(getFiles(addon), IsEmpty());
    EXPECT_THAT(getDeps(addon), IsEmpty());
}

TEST(AddonTest, DeserializeFullRemote)
{
    auto plugin   = get("plugins/;bin.smx");
    auto gamedata = get("gamedata/;data.txt");
    auto source   = get("scripting/;bin.sp");

    // clang-format off
    auto addon = json{
            {"id", "test"},
            {"author", "Somedev"},
            {"description", "Test addon"},
            {"explicit", true},
            {"files", {plugin->raw(), gamedata->raw(), source->raw()}},
            {"deps", {"someplugin", "someextension"}},
        }.get<std::shared_ptr<Addon>>();
    // clang-format on

    EXPECT_EQ("test", addon->getId());
    EXPECT_EQ("Somedev", addon->getAuthor());
    EXPECT_EQ("Test addon", addon->getDescription());
    EXPECT_TRUE(addon->isExplicit());

    EXPECT_THAT(getFiles(addon), ElementsAre(plugin, gamedata, source));

    EXPECT_THAT(getDeps(addon),
                UnorderedElementsAre("someplugin", "someextension"));
}

TEST(AddonTest, DeserializePartialRemote)
{
    // clang-format off
    auto addon = json{
            {"id", "test"},
            {"author", "Somedev"},
            {"description", "Test addon"},
        }.get<std::shared_ptr<Addon>>();
    // clang-format on

    EXPECT_EQ("test", addon->getId());
    EXPECT_EQ("Somedev", addon->getAuthor());
    EXPECT_EQ("Test addon", addon->getDescription());
    EXPECT_FALSE(addon->isExplicit());

    EXPECT_THAT(getFiles(addon), IsEmpty());
    EXPECT_THAT(getDeps(addon), IsEmpty());
}

TEST(AddonTest, SerializeFull)
{
    auto plugin   = get("plugins/bin.smx");
    auto gamedata = get("gamedata/data.txt");
    auto source   = get("scripting/bin.sp");

    // clang-format off
    auto addon = json{
            {"id", "test"},
            {"author", "Somedev"},
            {"description", "Test addon"},
            {"explicit", true},
            {"files", {plugin->raw(), gamedata->raw(), source->raw()}},
            {"deps", {"someplugin", "someextension"}},
        }.get<std::shared_ptr<Addon>>();
    // clang-format on

    json j = addon;

    EXPECT_EQ("test", j["id"]);
    EXPECT_EQ("Somedev", j["author"]);
    EXPECT_EQ("Test addon", j["description"]);
    EXPECT_TRUE(j["explicit"]);

    std::vector<File::Ptr> files;
    j["files"].get_to(files);
    EXPECT_THAT(files, ElementsAre(plugin, gamedata, source));

    std::set<std::string> deps;
    j["deps"].get_to(deps);
    EXPECT_THAT(deps,
                UnorderedElementsAre("someplugin", "someextension"));
}

TEST(AddonTest, SerializePartial)
{
    // clang-format off
    auto addon = json{
            {"id", "test"},
            {"author", "Somedev"},
            {"description", "Test addon"},
        }.get<std::shared_ptr<Addon>>();
    // clang-format on

    json j = addon;

    EXPECT_EQ("test", j["id"]);
    EXPECT_EQ("Somedev", j["author"]);
    EXPECT_EQ("Test addon", j["description"]);

    EXPECT_FALSE(j["explicit"]);
    EXPECT_THAT(j["files"], IsEmpty());
    EXPECT_THAT(j["deps"], IsEmpty());
}

// Note: Installation and removal tested by integration

TEST(AddonTest, Detach)
{
    auto plugin   = get("plugins/bin.smx");
    auto gamedata = get("gamedata/data.txt");
    auto source   = get("scripting/bin.sp");

    // clang-format off
    auto a = json{
            {"id", "test"},
            {"author", "Somedev"},
            {"description", "Test addon"},
            {"files", {plugin->raw(), gamedata->raw(), source->raw()}},
        }.get<std::shared_ptr<Addon>>();
    // clang-format on

    EXPECT_THAT(getFiles(a), ElementsAre(plugin, gamedata, source));

    a->detach(gamedata);
    EXPECT_THAT(getFiles(a), ElementsAre(plugin, source));

    a->detach(gamedata);
    EXPECT_THAT(getFiles(a), ElementsAre(plugin, source));

    a->detach(plugin);
    EXPECT_THAT(getFiles(a), ElementsAre(source));

    a->detach(plugin);
    EXPECT_THAT(getFiles(a), ElementsAre(source));

    a->detach(gamedata);
    EXPECT_THAT(getFiles(a), ElementsAre(source));

    a->detach(source);
    EXPECT_THAT(getFiles(a), IsEmpty());

    a->detach(plugin);
    EXPECT_THAT(getFiles(a), IsEmpty());

    a->detach(source);
    EXPECT_THAT(getFiles(a), IsEmpty());
}

TEST(AddonTest, FindByFile)
{
    auto plugin   = get("plugins/bin.smx");
    auto gamedata = get("gamedata/data.txt");
    auto source   = get("scripting/bin.sp");

    // clang-format off
    auto a1 = json{
        {"id", "test"},
        {"author", "Somedev"},
        {"description", "Test addon"},
        {"files", {plugin->raw(), gamedata->raw()}},
    }.get<std::shared_ptr<Addon>>();

    auto a2 = json{
        {"id", "test2"},
        {"author", "Somedev"},
        {"description", "Test addon2"},
        {"files", {gamedata->raw(), source->raw()}},
    }.get<std::shared_ptr<Addon>>();
    // clang-format on

    a1->addToInstalled();
    a2->addToInstalled();

    EXPECT_THAT(Addon::findByFile(plugin), ElementsAre(a1));
    EXPECT_THAT(Addon::findByFile(gamedata), ElementsAre(a1, a2));
    EXPECT_THAT(Addon::findByFile(source), ElementsAre(a2));
}

inline void testCommonJson(const json& j)
{
    ASSERT_TRUE(j.count("data"));
    ASSERT_TRUE(j.count("hash"));

    ASSERT_EQ(3810067853656611362u, j.at("hash").get<size_t>());
    ASSERT_EQ(2, j.at("data").size());

    const auto& a1 = j["data"][0];
    const auto& a2 = j["data"][1];

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
                ElementsAre("plugins/bin2.smx", "translations/tr.txt"));

    EXPECT_THAT(a2["deps"].get<std::vector<std::string>>(),
                ElementsAre("plugin2", "plugin3"));
}

TEST(AddonTest, SaveInstalled)
{
    Addon::clear();

    // clang-format off
    json{
        {"id", "test"},
        {"author", "Somedev"},
        {"description", "Test addon"},
        {"explicit", true},
        {"files", {"plugins/bin.smx", "gamedata/data.txt"}},
        {"deps", {"plugin1", "test2"}},
    }.get<std::shared_ptr<Addon>>()->addToInstalled();

    json{
        {"id", "test2"},
        {"author", "Somedev"},
        {"description", "Test addon2"},
        {"explicit", false},
        {"files", {"plugins/bin2.smx", "translations/tr.txt"}},
        {"deps", {"plugin2", "plugin3"}},
    }.get<std::shared_ptr<Addon>>()->addToInstalled();
    // clang-format on

    ASSERT_TRUE(Addon::save());

    std::ifstream ifs(dataFile);
    ASSERT_TRUE(ifs);

    json j = json::parse(ifs);
    ifs.close();

    testCommonJson(j);
}

TEST(AddonTest, LoadInstalled)
{
    Addon::clear();

    std::ofstream ofs(dataFile, std::ios::trunc);
    ASSERT_TRUE(ofs);

    // clang-format off
    ofs << json{{"data", {
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

    ofs.close();

    std::ifstream ifs(dataFile);
    ASSERT_TRUE(ifs);

    json j = json::parse(ifs);
    ifs.close();

    testCommonJson(j);
}
