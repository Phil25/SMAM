#include <operations/installer.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <net/database.h>

namespace smam
{
constexpr const char* url = "localhost:7666";

class OperarationInstallerTest : public ::testing::Test
{
protected:
    Logger   logger;
    AddonMap cache;

    void SetUp() override
    {
        cache = Database(logger, url, {"rtd", "tf2items"}).Cached();
    }
};

TEST_F(OperarationInstallerTest, CheckPending)
{
    auto exec  = Executor<InstallerContext>(logger, "rtd", cache);
    auto error = exec.Run<CheckPending>().GetError();

    ASSERT_FALSE(error) << error.message;

    error = exec.Run<CheckPending>().GetError();

    ASSERT_FALSE(error) << error.message;
    EXPECT_EQ("_", error.message);
}

TEST_F(OperarationInstallerTest, ParseCacheSuccess)
{
    auto exec  = Executor<InstallerContext>(logger, "rtd", cache);
    auto error = exec.Run<ParseCache>().GetError();

    ASSERT_FALSE(error) << error.message;

    const auto& addon = exec.GetContext().addon;
    ASSERT_TRUE(addon);

    EXPECT_EQ("rtd", addon->ID());
    EXPECT_EQ("https://github.com/Phil25/RTD/", addon->BaseURL());
    EXPECT_FALSE(addon->IsExplicit());  // not just yet
}

TEST_F(OperarationInstallerTest, ParseCacheFail)
{
    auto exec  = Executor<InstallerContext>(logger, "uncached", cache);
    auto error = exec.Run<ParseCache>().GetError();

    ASSERT_TRUE(error);
    EXPECT_EQ("Not found: \"uncached\"", error.message);
}

TEST_F(OperarationInstallerTest, CheckInstalled)
{
    auto exec = Executor<InstallerContext>(logger, "rtd", cache);
    auto error =
        exec.Run<ParseCache>().Run<CheckInstalled>(false).GetError();

    ASSERT_FALSE(error) << error.message;

    exec.GetContext().addon->MarkInstalled();
    error = exec.Run<CheckInstalled>(false).GetError();

    ASSERT_TRUE(error);
    EXPECT_EQ("Already installed: \"rtd\"", error.message);
}

TEST_F(OperarationInstallerTest, CheckInstalledForce)
{
    auto exec = Executor<InstallerContext>(logger, "tf2items", cache);
    auto error =
        exec.Run<ParseCache>().Run<CheckInstalled>(false).GetError();

    ASSERT_FALSE(error) << error.message;

    exec.GetContext().addon->MarkInstalled();
    error = exec.Run<CheckInstalled>(true).GetError();

    EXPECT_FALSE(error) << error.message;
}
}  // namespace smam
