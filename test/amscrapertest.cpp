#include "gtest/gtest.h"

#ifdef NDEBUG
#undef NDEBUG // make downloader use curlmock
#endif

#include "../src/scrapers/amscraper.h"

static Downloader dmock;
static AMScraper scraper(dmock);

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

TEST(AMScraperTest, AdvancedInfiniteAmmo)
{
	Attachments expected = {
		{
			"advancedinfiniteammo.inc",
			"https://forums.alliedmods.net/attachment.php?s=a59969c161c4e0ac23ef19c7cbb9ee5e&attachmentid=147135&d=1438556651"
		},
		{
			"AdvancedInfiniteAmmo.smx",
			"http://www.sourcemod.net/vbcompiler.php?file_id=148649"
		},
		{
			"AdvancedInfiniteAmmo.sp",
			"https://forums.alliedmods.net/attachment.php?s=a59969c161c4e0ac23ef19c7cbb9ee5e&attachmentid=148649&d=1444226746"
		},
	};

	Attachments actual = scraper.fetch("https://forums.alliedmods.net/showpost.php?p=1754217");

	compare(expected, actual);
}

TEST(AMScraperTest, AFKManager)
{
	Attachments expected = {
		{
			"afk_manager_old.zip",
			"https://forums.alliedmods.net/attachment.php?s=dc5547938c38333b03dff89b0ac17723&attachmentid=160240&d=1484963606"
		},
		{
			"afk_manager.phrases.txt",
			"https://forums.alliedmods.net/attachment.php?s=dc5547938c38333b03dff89b0ac17723&attachmentid=166646&d=1510967008"
		},
		{
			"afk_manager.inc",
			"https://forums.alliedmods.net/attachment.php?s=dc5547938c38333b03dff89b0ac17723&attachmentid=168028&d=1516358726"
		},
		{
			"afk_manager4.smx",
			"http://www.sourcemod.net/vbcompiler.php?file_id=170330"
		},
		{
			"afk_manager4.sp",
			"https://forums.alliedmods.net/attachment.php?s=dc5547938c38333b03dff89b0ac17723&attachmentid=170330&d=1530622367"
		},
		{
			"afk_manager.changelog.txt",
			"https://forums.alliedmods.net/attachment.php?s=dc5547938c38333b03dff89b0ac17723&attachmentid=170331&d=1530622381"
		},
	};

	Attachments actual = scraper.fetch("https://forums.alliedmods.net/showpost.php?p=708265");

	compare(expected, actual);
}

TEST(AMScraperTest, DynamicMotd)
{
	Attachments expected = {
		{
			"dynamic_motd.smx",
			"https://forums.alliedmods.net/attachment.php?s=a59969c161c4e0ac23ef19c7cbb9ee5e&attachmentid=160284&d=1485099001"
		},
		{
			"dynamic_motd.sp",
			"https://forums.alliedmods.net/attachment.php?s=a59969c161c4e0ac23ef19c7cbb9ee5e&attachmentid=160285&d=1485100105"
		},
	};

	Attachments actual = scraper.fetch("https://forums.alliedmods.net/showpost.php?p=1387386");

	compare(expected, actual);
}

TEST(AMScraperTest, FuncommandsX)
{

	Attachments expected = {
		{
			"funcommandsX_2.5.zip",
			"https://forums.alliedmods.net/attachment.php?s=dc5547938c38333b03dff89b0ac17723&attachmentid=159900&d=1483676185"
		},
	};

	Attachments actual = scraper.fetch("https://forums.alliedmods.net/showpost.php?p=665771");

	compare(expected, actual);
}

TEST(AMScraperTest, Thriller)
{
	Attachments expected = {
		{
			"thriller.plugin.txt",
			"https://forums.alliedmods.net/attachment.php?s=a59969c161c4e0ac23ef19c7cbb9ee5e&attachmentid=133555&d=1400274898"
		},
		{
			"thriller.smx",
			"http://www.sourcemod.net/vbcompiler.php?file_id=128466"
		},
		{
			"thriller.sp",
			"https://forums.alliedmods.net/attachment.php?s=a59969c161c4e0ac23ef19c7cbb9ee5e&attachmentid=128466&d=1386796191"
		},
	};

	Attachments actual = scraper.fetch("https://forums.alliedmods.net/showpost.php?p=1590169");

	compare(expected, actual);
}
