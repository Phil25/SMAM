#include <operations/installer.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <net/database.h>

#include <fstream>

namespace smam
{
constexpr const char* url = "localhost:7666";

class OperarationInstallerTest : public ::testing::Test
{
protected:
    LoggerPtr logger = std::make_shared<Logger>();
    AddonMap  cache;

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

TEST_F(OperarationInstallerTest, Transaction)
{
    namespace fs = std::filesystem;

    auto root = fs::current_path();

    auto exec  = Executor<InstallerContext>(logger, "rtd", cache);
    auto error = exec.Run<BeginTransaction>().GetError();

    ASSERT_FALSE(error) << error.message;

    ASSERT_EQ("/tmp/smam", fs::current_path());
    ASSERT_EQ(root, exec.GetContext().root);

    ASSERT_TRUE(fs::create_directory("plugins"));
    ASSERT_TRUE(fs::create_directory("gamedata"));
    std::ofstream("./gamedata/file.txt") << "Content";

    error = exec.Run<CommitTransaction>().GetError();

    EXPECT_FALSE(error) << error.message;
    EXPECT_FALSE(fs::exists("/tmp/smam"));

    ASSERT_EQ(root, fs::current_path());

    ASSERT_TRUE(fs::is_directory("plugins"));
    ASSERT_TRUE(fs::is_directory("gamedata"));
    ASSERT_TRUE(fs::exists("./gamedata/file.txt"));
    ASSERT_FALSE(fs::is_directory("./gamedata/file.txt"));

    auto content = std::string{};
    std::ifstream("./gamedata/file.txt") >> content;

    ASSERT_EQ("Content", content);

    fs::remove_all("plugins");
    fs::remove_all("gamedata");
}
}  // namespace smam
