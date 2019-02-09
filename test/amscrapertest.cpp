#include <string>
#include <vector>
#include <fstream>

#include "gtest/gtest.h"

#include "../src/scrapers/amscraper.h"

#include "downloadermock.h"

DownloaderMock dmock;
AMScraper scraper(dmock);

TEST(AMScraperTest, AdvancedInfiniteAmmo){
	scraper.fetch("https://forums.alliedmods.net/showpost.php?p=1754217");
	EXPECT_EQ(
		"http://www.sourcemod.net/vbcompiler.php?file_id=148649",
		scraper.getFileUrl("AdvancedInfiniteAmmo.sp", 'p')
	);
}

TEST(AMScraperTest, AFKManager){
	scraper.fetch("https://forums.alliedmods.net/showpost.php?p=708265");
	EXPECT_EQ(
		"http://afkmanager.dawgclan.net/plugins/afk_manager4.smx",
		scraper.getFileUrl("http://afkmanager.dawgclan.net/plugins/afk_manager4.smx", 'p')
	);
	EXPECT_EQ(
		"http://afkmanager.dawgclan.net/translations/afk_manager.phrases.txt",
		scraper.getFileUrl("http://afkmanager.dawgclan.net/translations/afk_manager.phrases.txt", 't')
	);
}

TEST(AMScraperTest, DynamicMotd){
	scraper.fetch("https://forums.alliedmods.net/showpost.php?p=1387386");
	EXPECT_EQ(
		"https://forums.alliedmods.net/attachment.php?attachmentid=160284",
		scraper.getFileUrl("dynamic_motd.smx", 'p')
	);
}

TEST(AMScraperTest, FuncommandsX){
	scraper.fetch("https://forums.alliedmods.net/showpost.php?p=665771");
	std::string name = scraper.getFileName("funcommandsX_*.zip");
	EXPECT_EQ(
		"https://forums.alliedmods.net/attachment.php?attachmentid=159903",
		scraper.getFileUrl(name, 'a')
	);
}

TEST(AMScraperTest, Thriller){
	scraper.fetch("https://forums.alliedmods.net/showpost.php?p=1590169");
	EXPECT_EQ(
		"http://www.sourcemod.net/vbcompiler.php?file_id=128466",
		scraper.getFileUrl("thriller.sp", 'p')
	);
	EXPECT_EQ(
		"https://forums.alliedmods.net/attachment.php?attachmentid=133555",
		scraper.getFileUrl("thriller.plugin.txt", 'g')
	);
}
