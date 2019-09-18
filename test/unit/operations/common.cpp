#include <operations/common.h>

#include <data/addon.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <nlohmann/json.hpp>

#include <filesystem>
#include <fstream>

namespace smam
{
using namespace testing;

class OperationsCommonTest : public ::testing::Test
{
protected:
    Logger                  logger;
    Executor<CommonContext> exec{logger};

    auto MakeOptions(std::string command) noexcept -> Options
    {
        std::vector<std::string> args{"smam"};
        std::string              arg;

        std::istringstream iss(std::move(command));
        while (iss >> arg) args.push_back(arg);

        int         size = args.size();
        const char* argv[size];

        for (int i = 0; i < size; ++i)
        {
            argv[i] = args[i].c_str();
        }

        return {size, argv, logger};
    }

    void SetUp() override
    {
        namespace fs = std::filesystem;
        fs::create_directories("mod/addons/sourcemod/plugins");
    }

    void TearDown() override
    {
        std::filesystem::remove_all("mod");
    }
};

TEST_F(OperationsCommonTest, CheckAddons)
{
    auto options1 = MakeOptions("install tf2items rtd");
    auto error1   = exec.Run<CheckAddons>(options1).GetError();

    ASSERT_FALSE(error1) << error1.message;

    auto options2 = MakeOptions("install -d abc");
    auto error2   = exec.Run<CheckAddons>(options2).GetError();

    ASSERT_TRUE(error2);
    EXPECT_EQ("No addons specified.", error2.message);
}

TEST_F(OperationsCommonTest, CheckSMRootNoOptions)
{
    auto error = exec.Run<CheckSMRoot>(MakeOptions("")).GetError();
    ASSERT_TRUE(error);
    EXPECT_EQ("SourceMod root not found.", error.message);
}

TEST_F(OperationsCommonTest, CheckSMRootNotFound)
{
    auto error = exec.Run<CheckSMRoot>(MakeOptions("-d .")).GetError();
    ASSERT_TRUE(error);
    EXPECT_EQ("SourceMod root not found.", error.message);
}

TEST_F(OperationsCommonTest, CheckSMRootFound)
{
    namespace fs               = std::filesystem;
    static const fs::path root = "mod/addons/sourcemod";

    auto error =
        exec.Run<CheckSMRoot>(MakeOptions("install tf2items -d ./mod/"))
            .GetError();

    ASSERT_FALSE(error) << error.message;
    EXPECT_TRUE(fs::equivalent(root, exec.GetContext().root));
}

TEST_F(OperationsCommonTest, LoadAddonsFinish)
{
    auto ofs = std::ofstream("mod/addons/sourcemod/.smamdata.json",
                             std::ios::trunc);
    ASSERT_TRUE(ofs);
    // clang-format off
    ofs << nlohmann::json{{"data", {
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

    auto error =
        exec.Run<LoadAddons>("mod/addons/sourcemod/.smamdata.json")
            .GetError();

    ASSERT_FALSE(error) << error.message;
}

TEST_F(OperationsCommonTest, LoadAddonsFail)
{
    auto ofs = std::ofstream("mod/addons/sourcemod/.smamdata.json",
                             std::ios::trunc);
    ASSERT_TRUE(ofs);
    ofs << "Wait, this isn't JSON...\n";
    ofs.close();

    auto error =
        exec.Run<LoadAddons>("mod/addons/sourcemod/.smamdata.json")
            .GetError();

    ASSERT_TRUE(error);
    EXPECT_EQ("Failed to load installed addons.", error.message);
}

TEST_F(OperationsCommonTest, SaveAddonsFinish)
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
        {"files", {"plugins/bin2.smx", "translations/tr.txt"}},
        {"deps", {"plugin2", "plugin3"}},
    }.get<AddonPtr>()->MarkInstalled();
    // clang-format on

    auto error =
        exec.Run<SaveAddons>("mod/addons/sourcemod/.smamdata.json")
            .GetError();

    ASSERT_FALSE(error) << error.message;

    auto ifs = std::ifstream("mod/addons/sourcemod/.smamdata.json");
    ASSERT_TRUE(ifs);

    auto json = nlohmann::json::parse(ifs);
    ifs.close();

    ASSERT_TRUE(json.count("data"));
    ASSERT_TRUE(json.count("hash"));

    ASSERT_EQ(3810067853656611362u, json.at("hash").get<size_t>());
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
                ElementsAre("plugins/bin2.smx", "translations/tr.txt"));

    EXPECT_THAT(a2["deps"].get<std::vector<std::string>>(),
                ElementsAre("plugin2", "plugin3"));
}
}  // namespace smam