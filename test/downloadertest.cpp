#include "gtest/gtest.h"

#ifdef NDEBUG
#undef NDEBUG // make downloader use curlmock
#endif

#define NDELAY // disable callback delay for curlmock

#include "../src/downloader.h"
#include <fstream>

class DownloaderTest : public ::testing::Test
{
protected:
	Downloader down;
};

int getFileSize(const char* path)
{
	std::ifstream ifs(path, std::ios::ate);
	int size = ifs.tellg();
	ifs.close();
	return size;
}

bool fileExists(const char* path)
{
	return std::ifstream(path).good();
}

TEST_F(DownloaderTest, TF2Items)
{
	auto result = down.html(
		"https://builds.limetech.io/?p=tf2items",
		"</h2>", "</div>"
	);
	EXPECT_EQ(10138, result.size()) << "Bad size";

	result = down.html(
		"https://builds.limetech.io/?p=tf2items",
		"<head>", "</head>"
	);
	EXPECT_EQ("\n\t\t<link href=\"/css/bootstrap.min.css\" rel=\"stylesheet\">\n\t", result);
}

TEST_F(DownloaderTest, Thriller)
{
	auto result = down.html(
		"https://forums.alliedmods.net/showpost.php?p=1590169",
		"<!-- attachments -->", "<!-- / attachments -->"
	);
	EXPECT_EQ(1163, result.size()) << "Bad size";

	result = down.html(
		"https://forums.alliedmods.net/showpost.php?p=1590169",
		"<a href=", ">"
	);
	EXPECT_EQ("\"member.php?s=a59969c161c4e0ac23ef19c7cbb9ee5e&amp;u=38829\"", result);
}

TEST_F(DownloaderTest, TF2Attributes)
{
	auto result = down.html("https://api.github.com/repos/FlaminSarge/tf2attributes/releases/latest");
	EXPECT_EQ(5655, result.size()) << "Bad size";

	result = down.html(
		"https://api.github.com/repos/FlaminSarge/tf2attributes/releases/latest",
		"node_id\": \"", "=\""
	);
	EXPECT_EQ("MDc6UmVsZWFzZTE0OTE2MjE", result);
}

TEST_F(DownloaderTest, TF2ItemsFiles)
{
	ASSERT_TRUE(down.file("https://builds.limetech.io/files/tf2items-1.6.4-hg279-linux.zip", "tf2items-1.6.4-hg279-linux.zip").empty());
	ASSERT_TRUE(fileExists("tf2items-1.6.4-hg279-linux.zip"));
	EXPECT_EQ(77707, getFileSize("tf2items-1.6.4-hg279-linux.zip"));
}

TEST_F(DownloaderTest, ThrillerFiles)
{
	ASSERT_TRUE(down.file("http://www.sourcemod.net/vbcompiler.php?file_id=128466", "thriller.smx").empty());
	ASSERT_TRUE(fileExists("thriller.smx"));
	EXPECT_EQ(3691, getFileSize("thriller.smx"));

	ASSERT_TRUE(down.file("https://forums.alliedmods.net/attachment.php?s=a59969c161c4e0ac23ef19c7cbb9ee5e&attachmentid=133555&d=1400274898", "thriller.plugin.txt").empty());
	ASSERT_TRUE(fileExists("thriller.plugin.txt"));
	EXPECT_EQ(656, getFileSize("thriller.plugin.txt"));
}

TEST_F(DownloaderTest, TF2AttributesFiles)
{
	ASSERT_TRUE(down.file("https://github.com/FlaminSarge/tf2attributes/releases/download/v1.2.1/tf2attributes.smx", "tf2attributes.smx").empty());
	ASSERT_TRUE(fileExists("tf2attributes.smx"));
	EXPECT_EQ(12701, getFileSize("tf2attributes.smx"));

	ASSERT_TRUE(down.file("https://raw.githubusercontent.com/FlaminSarge/tf2attributes/master/tf2.attributes.txt", "tf2.attributes.txt").empty());
	ASSERT_TRUE(fileExists("tf2.attributes.txt"));
	EXPECT_EQ(3287, getFileSize("tf2.attributes.txt"));
}

TEST_F(DownloaderTest, InvalidLink)
{
	ASSERT_FALSE(down.file("invalid link", "somefile.bin").empty());
	EXPECT_FALSE(fileExists("somefile.bin"));
}
