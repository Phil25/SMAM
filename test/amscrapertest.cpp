#include <string>
#include <vector>
#include <fstream>

#include "gtest/gtest.h"

#include "../src/scrapers/amscraper.h"
#include "../src/utils.h"

#include "downloadermock.h"

class AMScraperTest : public ::testing::Test{
protected:
	std::vector<AMScraper*> scrapers;
	std::vector<std::string> pluginName;
	DownloaderMock dmock;

	void SetUp() override{
		for(auto& entry : DownloaderMock::dataLink){
			AMScraper* scraper = new AMScraper(dmock);
			scraper->getPage(entry.first);

			scrapers.push_back(scraper);
			pluginName.push_back(entry.second);
		}
	}

	void TearDown() override{
		for(AMScraper* scraper : scrapers)
			delete scraper;
	}

	int getPluinOffset(const std::string& name){
		int size = pluginName.size();
		for(int i = 0; i < size; ++i)
			if(name == pluginName.at(i))
				return i;
		return 0;
	}
};

TEST_F(AMScraperTest, AdvancedInfiniteAmmo){
	int i = getPluinOffset("advancedinfiniteammo");
	EXPECT_EQ(
		"http://www.sourcemod.net/vbcompiler.php?file_id=148649",
		scrapers[i]->getFileUrl("AdvancedInfiniteAmmo.sp", 'p')
	);
}

TEST_F(AMScraperTest, AFKManager){
	int i = getPluinOffset("afk_manager");
	EXPECT_EQ(
		"http://afkmanager.dawgclan.net/plugins/afk_manager4.smx",
		scrapers[i]->getFileUrl("http://afkmanager.dawgclan.net/plugins/afk_manager4.smx", 'p')
	);
	EXPECT_EQ(
		"http://afkmanager.dawgclan.net/translations/afk_manager.phrases.txt",
		scrapers[i]->getFileUrl("http://afkmanager.dawgclan.net/translations/afk_manager.phrases.txt", 't')
	);
}

TEST_F(AMScraperTest, DynamicMotd){
	int i = getPluinOffset("dynamic_motd");
	EXPECT_EQ(
		"https://forums.alliedmods.net/attachment.php?attachmentid=160284",
		scrapers[i]->getFileUrl("dynamic_motd.smx", 'p')
	);
}

TEST_F(AMScraperTest, FuncommandsX){
	int i = getPluinOffset("funcommandsx");
	EXPECT_EQ(
		"https://forums.alliedmods.net/attachment.php?attachmentid=159903",
		scrapers[i]->getFileUrl("funcommandsX_*.zip", 'a')
	);
}

TEST_F(AMScraperTest, Thriller){
	int i = getPluinOffset("thriller");
	EXPECT_EQ(
		"http://www.sourcemod.net/vbcompiler.php?file_id=128466",
		scrapers[i]->getFileUrl("thriller.sp", 'p')
	);
	EXPECT_EQ(
		"https://forums.alliedmods.net/attachment.php?attachmentid=133555",
		scrapers[i]->getFileUrl("thriller.plugin.txt", 'g')
	);
}
