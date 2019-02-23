#include "gtest/gtest.h"

#include "../src/scrapers/ltscraper.h"

#include "downloadermock.h"

static DownloaderMock dmock;
static LTScraper scraper(dmock);

inline void compare(const Attachments& expected, const Attachments& actual)
{
	ASSERT_EQ(expected.size(), actual.size());

	for(const auto& [name, url] : actual)
	{
		auto entry = expected.find(name);
		ASSERT_NE(expected.end(), entry) << "Could not find " << name;
		EXPECT_EQ(entry->second, url);
	}
}

TEST(LTScraperTest, Accelerator)
{
	Attachments expected = {
		{
			"accelerator-2.4.3-git127-b302f00-linux.zip",
			"https://builds.limetech.io/files/accelerator-2.4.3-git127-b302f00-linux.zip"
		}
	};

	Attachments actual = scraper.fetch("https://builds.limetech.io/?p=accelerator");

	compare(expected, actual);
}

TEST(LTScraperTest, Connect)
{
	Attachments expected = {
		{
			"connect-1.2.0-hg38-linux.zip",
			"https://builds.limetech.io/files/connect-1.2.0-hg38-linux.zip"
		}
	};

	Attachments actual = scraper.fetch("https://builds.limetech.io/?p=connect");

	compare(expected, actual);
}

TEST(LTScraperTest, SteamTools)
{
	Attachments expected = {
		{
			"steamtools-0.10.0-git179-54fdc51-linux.zip",
			"https://builds.limetech.io/files/steamtools-0.10.0-git179-54fdc51-linux.zip"
		}
	};

	Attachments actual = scraper.fetch("https://builds.limetech.io/?p=steamtools");

	compare(expected, actual);
}

TEST(LTScraperTest, TF2Items)
{
	Attachments expected = {
		{
			"tf2items-1.6.4-hg279-linux.zip",
			"https://builds.limetech.io/files/tf2items-1.6.4-hg279-linux.zip"
		}
	};

	Attachments actual = scraper.fetch("https://builds.limetech.io/?p=tf2items");

	compare(expected, actual);
}
