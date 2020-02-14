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
    std::filesystem::path current;

protected:
    LoggerPtr logger = std::make_shared<Logger>();

    auto Exec(const char* cmd) noexcept -> Executor<CommonContext>
    {
        std::vector<std::string> args{"smam"};
        std::string              arg;

        std::istringstream iss(cmd);
        while (iss >> arg) args.push_back(arg);

        int         size = args.size();
        const char* argv[size];

        for (int i = 0; i < size; ++i)
        {
            argv[i] = args[i].c_str();
        }

        return {logger,
                std::make_shared<Options>(Options{size, argv, logger})};
    }

    auto Exec() noexcept -> Executor<CommonContext>
    {
        return Exec("");
    }

    void SetUp() override
    {
        namespace fs = std::filesystem;
        fs::create_directories("./mod/addons/sourcemod/plugins");
        this->current = fs::current_path();
    }

    void TearDown() override
    {
        namespace fs = std::filesystem;
        fs::current_path(this->current);
        fs::remove_all("./mod/");
        logger->SetOutput(true);
    }
};

TEST_F(OperationsCommonTest, CheckAddons)
{
    auto error1 =
        Exec("install tf2items rtd").Run<CheckAddons>().GetError();

    ASSERT_FALSE(error1) << error1.message;

    auto error2 = Exec("install -d abc").Run<CheckAddons>().GetError();

    ASSERT_TRUE(error2);
    EXPECT_EQ("No addons specified.", error2.message);
}

TEST_F(OperationsCommonTest, GoToSMRootNoOptions)
{
    auto error = Exec().Run<GoToSMRoot>().GetError();
    ASSERT_TRUE(error);
    EXPECT_EQ("SourceMod root not found.", error.message);
}

TEST_F(OperationsCommonTest, GoToSMRootNotFound)
{
    auto error = Exec("-d .").Run<GoToSMRoot>().GetError();
    ASSERT_TRUE(error);
    EXPECT_EQ("SourceMod root not found.", error.message);
}

TEST_F(OperationsCommonTest, GoToSMRootFound)
{
    namespace fs = std::filesystem;
    static const fs::path root =
        fs::current_path() / "mod/addons/sourcemod";

    auto exec  = Exec("install tf2items -d ./mod/");
    auto error = exec.Run<GoToSMRoot>().GetError();

    ASSERT_FALSE(error) << error.message;
    EXPECT_TRUE(fs::equivalent(root, exec.GetContext().root));
    EXPECT_TRUE(
        fs::equivalent(fs::current_path(), exec.GetContext().root));
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

    std::filesystem::current_path("./mod/addons/sourcemod/");
    auto error = Exec().Run<LoadAddons>(".smamdata.json").GetError();

    ASSERT_FALSE(error) << error.message;
}

TEST_F(OperationsCommonTest, LoadAddonsFailGeneric)
{
    logger->SetOutput(false);

    auto ofs = std::ofstream("mod/addons/sourcemod/.smamdata.json",
                             std::ios::trunc);
    ASSERT_TRUE(ofs);
    ofs << "Wait, this isn't JSON...\n";
    ofs.close();

    ofs = std::ofstream("mod/addons/sourcemod/.smamdata.json.bak",
                        std::ios::trunc);
    ASSERT_TRUE(ofs);
    ofs << "Wait, this isn't JSON...\n";
    ofs.close();

    std::filesystem::current_path("./mod/addons/sourcemod/");
    auto error = Exec().Run<LoadAddons>(".smamdata.json").GetError();

    ASSERT_TRUE(error);
    EXPECT_EQ("Failed to load installed addons.", error.message);
}

TEST_F(OperationsCommonTest, LoadAddonsFailBackupNotExists)
{
    logger->SetOutput(false);

    auto ofs = std::ofstream("mod/addons/sourcemod/.smamdata.json",
                             std::ios::trunc);
    ASSERT_TRUE(ofs);
    ofs << "Wait, this isn't JSON...\n";
    ofs.close();

    std::filesystem::current_path("./mod/addons/sourcemod/");
    auto error = Exec().Run<LoadAddons>(".smamdata.json").GetError();

    ASSERT_TRUE(error);
    EXPECT_EQ("Backup does not exist.", error.message);
}

TEST_F(OperationsCommonTest, SaveAddons)
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
        Exec()
            .Run<SaveAddons>("mod/addons/sourcemod/.smamdata.json")
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

TEST_F(OperationsCommonTest, SaveAddonsBackup)
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
        Exec()
            .Run<SaveAddons>("mod/addons/sourcemod/.smamdata.json")
            .GetError();

    ASSERT_FALSE(error) << error.message;

    namespace fs = std::filesystem;
    EXPECT_TRUE(fs::exists("mod/addons/sourcemod/.smamdata.json"));
    EXPECT_TRUE(fs::exists("mod/addons/sourcemod/.smamdata.json.bak"));
}

TEST_F(OperationsCommonTest, LoadAddonsBackup)
{
    logger->SetOutput(false);

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
    }}, {"hash", 3810067853656611361u}};  // NOTE: invalid hash
    // clang-format on
    ofs.close();

    ofs = std::ofstream("mod/addons/sourcemod/.smamdata.json.bak",
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
    }}, {"hash", 3810067853656611362u}};  // NOTE: correct hash
    // clang-format on
    ofs.close();

    std::filesystem::current_path("./mod/addons/sourcemod/");
    auto error = Exec().Run<LoadAddons>(".smamdata.json").GetError();

    ASSERT_FALSE(error) << error.message;
}
}  // namespace smam
