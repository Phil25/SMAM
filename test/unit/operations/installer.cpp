#include <operations/installer.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace smam
{
constexpr const char* url = "localhost:7666";

class InstallerTest : public ::testing::Test
{
protected:
    Logger                     logger;
    Executor<InstallerContext> exec{logger};
};

TEST_F(InstallerTest, PrecacheAddonsSinglePositive)
{
    const auto addons = std::vector<std::string>{"accelerator"};
    const auto error = exec.Run<PrecacheAddons>(url, addons).GetError();

    ASSERT_FALSE(error) << error.message;

    const auto& data = exec.GetContext().data;

    ASSERT_EQ(1, data.size());
    ASSERT_TRUE(data.count("accelerator"));

    const auto& plan = data.at("accelerator");

    EXPECT_EQ("https://builds.limetech.io/?p=accelerator", plan.url);
    ASSERT_TRUE(plan.addon);

    EXPECT_EQ("accelerator", plan.addon->ID());
    EXPECT_TRUE(plan.addon->IsExplicit());
}

TEST_F(InstallerTest, PrecacheAddonsSingleNegative)
{
    const auto addons = std::vector<std::string>{"invalid"};
    const auto error = exec.Run<PrecacheAddons>(url, addons).GetError();

    ASSERT_FALSE(error) << error.message;

    const auto& data = exec.GetContext().data;

    ASSERT_EQ(0, data.size());
}

TEST_F(InstallerTest, PrecacheAddonsMultiple)
{
    const auto addons =
        std::vector<std::string>{"accelerator", "invalid", "thriller"};
    const auto error = exec.Run<PrecacheAddons>(url, addons).GetError();

    ASSERT_FALSE(error) << error.message;

    const auto& data = exec.GetContext().data;

    ASSERT_EQ(2, data.size());
    ASSERT_TRUE(data.count("accelerator"));
    ASSERT_TRUE(data.count("thriller"));

    const auto& acc = data.at("accelerator");
    const auto& thr = data.at("thriller");

    EXPECT_EQ("https://builds.limetech.io/?p=accelerator", acc.url);
    EXPECT_EQ("https://forums.alliedmods.net/showpost.php?p=1590169",
              thr.url);

    ASSERT_TRUE(acc.addon);
    ASSERT_TRUE(thr.addon);

    EXPECT_EQ("accelerator", acc.addon->ID());
    EXPECT_EQ("thriller", thr.addon->ID());

    EXPECT_TRUE(acc.addon->IsExplicit());
    EXPECT_TRUE(thr.addon->IsExplicit());
}

TEST_F(InstallerTest, PrecacheAddonsDependencies)
{
    const auto addons = std::vector<std::string>{"rtd", "a_wants_b"};
    const auto error = exec.Run<PrecacheAddons>(url, addons).GetError();

    ASSERT_FALSE(error) << error.message;

    const auto& data = exec.GetContext().data;

    ASSERT_EQ(4, data.size());
    ASSERT_TRUE(data.count("rtd"));
    ASSERT_TRUE(data.count("tf2attributes"));
    ASSERT_TRUE(data.count("a_wants_b"));
    ASSERT_TRUE(data.count("b_wants_a"));

    const auto& rtd = data.at("rtd");
    const auto& tf2 = data.at("tf2attributes");
    const auto& awb = data.at("a_wants_b");
    const auto& bwa = data.at("b_wants_a");

    EXPECT_EQ("https://github.com/Phil25/RTD/", rtd.url);
    EXPECT_EQ("https://github.com/FlaminSarge/tf2attributes/", tf2.url);
    EXPECT_EQ("https://forums.alliedmods.net/showpost.php?p=1590169",
              awb.url);
    EXPECT_EQ("https://forums.alliedmods.net/showpost.php?p=1590169",
              bwa.url);

    ASSERT_TRUE(rtd.addon);
    ASSERT_TRUE(tf2.addon);
    ASSERT_TRUE(awb.addon);
    ASSERT_TRUE(bwa.addon);

    EXPECT_EQ("rtd", rtd.addon->ID());
    EXPECT_EQ("tf2attributes", tf2.addon->ID());
    EXPECT_EQ("a_wants_b", awb.addon->ID());
    EXPECT_EQ("b_wants_a", bwa.addon->ID());

    EXPECT_TRUE(rtd.addon->IsExplicit());
    EXPECT_TRUE(awb.addon->IsExplicit());

    EXPECT_FALSE(tf2.addon->IsExplicit());
    EXPECT_FALSE(bwa.addon->IsExplicit());
}

TEST_F(InstallerTest, InitScrapers)
{
    const auto error = exec.Run<InitScrapers>().GetError();

    ASSERT_FALSE(error) << error.message;
    ASSERT_EQ(3, exec.GetContext().scrapers.size());

    ASSERT_TRUE(exec.GetContext().scrapers[0]);
    EXPECT_EQ("https://forums.alliedmods.net/",
              exec.GetContext().scrapers[0]->Url());

    ASSERT_TRUE(exec.GetContext().scrapers[1]);
    EXPECT_EQ("https://builds.limetech.io/",
              exec.GetContext().scrapers[1]->Url());

    ASSERT_TRUE(exec.GetContext().scrapers[2]);
    EXPECT_EQ("https://github.com/",
              exec.GetContext().scrapers[2]->Url());
}

TEST_F(InstallerTest, CheckPending)
{
    auto error = exec.Run<CheckPending>("id").GetError();

    ASSERT_FALSE(error) << error.message;

    error = exec.Run<CheckPending>("id").GetError();

    ASSERT_FALSE(error) << error.message;
    EXPECT_EQ("_", error.message);
}

TEST_F(InstallerTest, SetAddonSuccess)
{
    auto addons = std::vector<std::string>{"rtd"};

    auto error = exec.Run<PrecacheAddons>(url, addons)
                     .Run<SetAddon>("rtd")
                     .GetError();

    ASSERT_FALSE(error) << error.message;

    const auto& addon = exec.GetContext().addon;
    const auto& url   = exec.GetContext().url;

    ASSERT_TRUE(addon);
    EXPECT_EQ("https://github.com/Phil25/RTD/", url);
    EXPECT_EQ("rtd", addon->ID());
    EXPECT_TRUE(addon->IsExplicit());
}

TEST_F(InstallerTest, SetAddonFail)
{
    auto error = exec.Run<SetAddon>("rtd").GetError();

    ASSERT_TRUE(error);
    EXPECT_EQ("Not found: \"rtd\"", error.message);
}

TEST_F(InstallerTest, CheckInstalled)
{
    auto addons = std::vector<std::string>{"rtd"};

    auto error = exec.Run<PrecacheAddons>(url, addons)
                     .Run<SetAddon>("rtd")
                     .GetError();

    ASSERT_FALSE(error) << error.message;

    error = exec.Run<CheckInstalled>(false).GetError();
    ASSERT_FALSE(error) << error.message;

    exec.GetContext().addon->MarkInstalled();

    error = exec.Run<CheckInstalled>(false).GetError();
    ASSERT_TRUE(error);
    EXPECT_EQ("Already installed: \"rtd\"", error.message);
}

TEST_F(InstallerTest, CheckInstalledForce)
{
    auto addons = std::vector<std::string>{"rtd"};

    auto error = exec.Run<PrecacheAddons>(url, addons)
                     .Run<SetAddon>("rtd")
                     .GetError();

    ASSERT_FALSE(error) << error.message;

    error = exec.Run<CheckInstalled>(true).GetError();
    ASSERT_FALSE(error) << error.message;

    exec.GetContext().addon->MarkInstalled();

    error = exec.Run<CheckInstalled>(true).GetError();
    EXPECT_FALSE(error) << error.message;
}
}  // namespace smam
