#include "gtest/gtest.h"

#include "../src/scrapers/ghscraper.h"

#include "downloadermock.h"

static DownloaderMock dmock;
static GHScraper scraper(dmock);

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

TEST(GHScraperTest, TF2Attributes)
{
	Attachments expected = {
		{
			"tf2attributes.smx",
			"https://github.com/FlaminSarge/tf2attributes/releases/download/v1.2.1/tf2attributes.smx"
		},
		{
			"tf2attributes_example.smx",
			"https://github.com/FlaminSarge/tf2attributes/releases/download/v1.2.1/tf2attributes_example.smx"
		},
	};

	Attachments actual = scraper.fetch("https://github.com/FlaminSarge/tf2attributes/");

	compare(expected, actual);
}
