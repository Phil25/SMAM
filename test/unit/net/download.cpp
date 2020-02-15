#include <net/download.h>

#include <gtest/gtest.h>

#ifdef NDEBUG
#undef NDEBUG  // make download use curlmock
#endif

#define NDELAY  // disable callback delay for curlmock

#include <utils/common.h>

#include <filesystem>
#include <fstream>

namespace smam
{
class DownloadTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        if (!std::filesystem::exists("./mod/"))
            ASSERT_TRUE(std::filesystem::create_directories("./mod/"));
    }

    void TearDown() override
    {
        std::filesystem::remove_all("./mod/");
    }
};

TEST_F(DownloadTest, TF2Items)
{
    auto result =
        download::Html("https://builds.limetech.io/?p=tf2items").str();

    ASSERT_EQ(10716, result.size()) << "Bad size";

    result = utils::ExtractString(result, "<head>", "</head>");
    EXPECT_EQ(
        "\n\t\t<link href=\"/css/bootstrap.min.css\" "
        "rel=\"stylesheet\">\n\t",
        result);
}

TEST_F(DownloadTest, Thriller)
{
    auto result =
        download::Html(
            "https://forums.alliedmods.net/showpost.php?p=1590169")
            .str();

    ASSERT_EQ(20009, result.size()) << "Bad size";

    result = utils::ExtractString(result, "<a href=", ">");
    EXPECT_EQ(
        "\"member.php?s=a59969c161c4e0ac23ef19c7cbb9ee5e&amp;u=38829\"",
        result);
}

TEST_F(DownloadTest, TF2Attributes)
{
    auto result =
        download::Html(
            "https://api.github.com/repos/FlaminSarge/tf2attributes/"
            "releases/latest")
            .str();

    ASSERT_EQ(5655, result.size()) << "Bad size";

    result = utils::ExtractString(result, "node_id\": \"", "=\"");
    EXPECT_EQ("MDc6UmVsZWFzZTE0OTE2MjE", result);
}

TEST_F(DownloadTest, TF2ItemsFiles)
{
    namespace fs = std::filesystem;

    auto error = download::File(
        "https://builds.limetech.io/files/"
        "tf2items-1.6.4-hg279-linux.zip",
        "./mod/tf2items-1.6.4-hg279-linux.zip");

    ASSERT_FALSE(error) << error.message;

    ASSERT_TRUE(fs::exists("./mod/tf2items-1.6.4-hg279-linux.zip"));
    EXPECT_EQ(77707,
              fs::file_size("./mod/tf2items-1.6.4-hg279-linux.zip"));
}

TEST_F(DownloadTest, ThrillerFiles)
{
    namespace fs = std::filesystem;

    auto error = download::File(
        "http://www.sourcemod.net/"
        "vbcompiler.php?file_id=128466",
        "./mod/thriller.smx");

    ASSERT_FALSE(error) << error.message;

    ASSERT_TRUE(fs::exists("./mod/thriller.smx"));
    EXPECT_EQ(3691, fs::file_size("./mod/thriller.smx"));

    error = download::File(
        "https://forums.alliedmods.net/"
        "attachment.php?s=a59969c161c4e0ac23ef19c7cbb9ee5e&"
        "attachmentid=133555&d=1400274898",
        "./mod/thriller.plugin.txt");

    ASSERT_FALSE(error) << error.message;

    ASSERT_TRUE(fs::exists("./mod/thriller.plugin.txt"));
    EXPECT_EQ(656, fs::file_size("./mod/thriller.plugin.txt"));
}

TEST_F(DownloadTest, TF2AttributesFiles)
{
    namespace fs = std::filesystem;

    auto error = download::File(
        "https://github.com/FlaminSarge/tf2attributes/"
        "releases/download/v1.2.1/tf2attributes.smx",
        "./mod/tf2attributes.smx");

    ASSERT_FALSE(error) << error.message;

    ASSERT_TRUE(fs::exists("./mod/tf2attributes.smx"));
    EXPECT_EQ(12701, fs::file_size("./mod/tf2attributes.smx"));

    error = download::File(
        "https://raw.githubusercontent.com/FlaminSarge/"
        "tf2attributes/master/tf2.attributes.txt",
        "./mod/tf2.attributes.txt");

    ASSERT_FALSE(error) << error.message;

    ASSERT_TRUE(fs::exists("./mod/tf2.attributes.txt"));
    EXPECT_EQ(3287, fs::file_size("./mod/tf2.attributes.txt"));
}

TEST_F(DownloadTest, InvalidLink)
{
    auto error = download::File("invalid link", "./mod/somefile.bin");

    ASSERT_TRUE(error);
    EXPECT_EQ("Not found: \"invalid link\"", error.message);
    EXPECT_FALSE(std::filesystem::exists("somefile.bin"));
}
}  // namespace smam
