#include "gtest/gtest.h"

#ifdef NDEBUG
#undef NDEBUG // make downloader use curlmock
#endif

#define NDELAY // disable callback delay for curlmock

#include "../src/downloader.h"

class DownloaderTest : public ::testing::Test
{
protected:
	Downloader down;
};

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
