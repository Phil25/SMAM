#include <gtest/gtest.h>

#ifdef NDEBUG
#undef NDEBUG  // make download use curlmock
#endif

#define NDELAY  // disable callback delay for curlmock

#include <fstream>
#include "../src/download.h"

int getFileSize(const char* path) noexcept
{
    std::ifstream ifs(path, std::ios::ate);
    int           size = ifs.tellg();
    ifs.close();
    return size;
}

bool fileExists(const char* path) noexcept
{
    return std::ifstream(path).good();
}

TEST(DownloadTest, TF2Items)
{
    auto result = Download::page(
        "https://builds.limetech.io/?p=tf2items", "</h2>", "</div>");
    EXPECT_EQ(10138, result.size()) << "Bad size";

    result = Download::page("https://builds.limetech.io/?p=tf2items",
                            "<head>", "</head>");
    EXPECT_EQ(
        "\n\t\t<link href=\"/css/bootstrap.min.css\" "
        "rel=\"stylesheet\">\n\t",
        result);
}

TEST(DownloadTest, Thriller)
{
    auto result = Download::page(
        "https://forums.alliedmods.net/showpost.php?p=1590169",
        "<!-- attachments -->", "<!-- / attachments -->");
    EXPECT_EQ(1163, result.size()) << "Bad size";

    result = Download::page(
        "https://forums.alliedmods.net/showpost.php?p=1590169",
        "<a href=", ">");
    EXPECT_EQ(
        "\"member.php?s=a59969c161c4e0ac23ef19c7cbb9ee5e&amp;u=38829\"",
        result);
}

TEST(DownloadTest, TF2Attributes)
{
    auto result = Download::page(
        "https://api.github.com/repos/FlaminSarge/tf2attributes/"
        "releases/latest");
    EXPECT_EQ(5655, result.size()) << "Bad size";

    result = Download::page(
        "https://api.github.com/repos/FlaminSarge/tf2attributes/"
        "releases/latest",
        "node_id\": \"", "=\"");
    EXPECT_EQ("MDc6UmVsZWFzZTE0OTE2MjE", result);
}

TEST(DownloadTest, TF2ItemsFiles)
{
    ASSERT_TRUE(Download::file("https://builds.limetech.io/files/"
                               "tf2items-1.6.4-hg279-linux.zip",
                               "tf2items-1.6.4-hg279-linux.zip")
                    .empty());
    ASSERT_TRUE(fileExists("tf2items-1.6.4-hg279-linux.zip"));
    EXPECT_EQ(77707, getFileSize("tf2items-1.6.4-hg279-linux.zip"));
}

TEST(DownloadTest, ThrillerFiles)
{
    ASSERT_TRUE(Download::file("http://www.sourcemod.net/"
                               "vbcompiler.php?file_id=128466",
                               "thriller.smx")
                    .empty());
    ASSERT_TRUE(fileExists("thriller.smx"));
    EXPECT_EQ(3691, getFileSize("thriller.smx"));

    ASSERT_TRUE(Download::file(
                    "https://forums.alliedmods.net/"
                    "attachment.php?s=a59969c161c4e0ac23ef19c7cbb9ee5e&"
                    "attachmentid=133555&d=1400274898",
                    "thriller.plugin.txt")
                    .empty());
    ASSERT_TRUE(fileExists("thriller.plugin.txt"));
    EXPECT_EQ(656, getFileSize("thriller.plugin.txt"));
}

TEST(DownloadTest, TF2AttributesFiles)
{
    ASSERT_TRUE(
        Download::file("https://github.com/FlaminSarge/tf2attributes/"
                       "releases/download/v1.2.1/tf2attributes.smx",
                       "tf2attributes.smx")
            .empty());
    ASSERT_TRUE(fileExists("tf2attributes.smx"));
    EXPECT_EQ(12701, getFileSize("tf2attributes.smx"));

    ASSERT_TRUE(
        Download::file("https://raw.githubusercontent.com/FlaminSarge/"
                       "tf2attributes/master/tf2.attributes.txt",
                       "tf2.attributes.txt")
            .empty());
    ASSERT_TRUE(fileExists("tf2.attributes.txt"));
    EXPECT_EQ(3287, getFileSize("tf2.attributes.txt"));
}

TEST(DownloadTest, InvalidLink)
{
    ASSERT_FALSE(
        Download::file("invalid link", "somefile.bin").empty());
    EXPECT_FALSE(fileExists("somefile.bin"));
}
