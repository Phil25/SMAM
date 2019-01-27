#include "gtest/gtest.h"

#include "../src/plan.h"
#include "../src/scrapers/amscraper.h"

#include "downloadermock.h"
#include "databasemock.h"

class PlanTest : public ::testing::Test{
protected:
	DatabaseMock db;
	DownloaderMock downloader;

	Plan thriller, funcommandsx, afk_manager;

	void SetUp() override{
		thriller.setId("thriller");
		funcommandsx.setId("funcommandsx");
		afk_manager.setId("afk_manager");
	}
};

TEST_F(PlanTest, ThrillerInit){
	thriller.init(db);
	EXPECT_EQ("https://forums.alliedmods.net/showpost.php?p=1590169", thriller.getUrl());
	ASSERT_EQ(2, thriller.size());

	EXPECT_EQ('p', thriller.getFileTag(0));
	EXPECT_EQ('g', thriller.getFileTag(1));

	EXPECT_EQ("plugins", thriller.getFileDir(0));
	EXPECT_EQ("gamedata", thriller.getFileDir(1));

	EXPECT_EQ("thriller.sp", thriller.getFileName(0));
	EXPECT_EQ("thriller.plugin.txt", thriller.getFileName(1));
}

TEST_F(PlanTest, ThrillerFetch){
	thriller.init(db);
	AMScraper scraper(downloader);
	thriller.fetch(scraper);

	ASSERT_EQ(2, thriller.size());
	EXPECT_EQ("http://www.sourcemod.net/vbcompiler.php?file_id=128466", thriller.getFileUrl(0));
	EXPECT_EQ("https://forums.alliedmods.net/attachment.php?attachmentid=133555", thriller.getFileUrl(1));
}

TEST_F(PlanTest, FuncommandsxInit){
	funcommandsx.init(db);
	EXPECT_EQ("https://forums.alliedmods.net/showpost.php?p=665771", funcommandsx.getUrl());
	ASSERT_EQ(1, funcommandsx.size());

	EXPECT_EQ('a', funcommandsx.getFileTag(0));
	EXPECT_EQ("", funcommandsx.getFileDir(0));
	EXPECT_EQ("funcommandsX_*.zip", funcommandsx.getFileName(0));
}

TEST_F(PlanTest, FuncommandsxFetch){
	funcommandsx.init(db);
	AMScraper scraper(downloader);
	funcommandsx.fetch(scraper);

	ASSERT_EQ(1, funcommandsx.size());
	EXPECT_EQ("funcommandsX_2.2.8.zip", funcommandsx.getFileName(0));
	EXPECT_EQ("https://forums.alliedmods.net/attachment.php?attachmentid=159903", funcommandsx.getFileUrl(0));
}

TEST_F(PlanTest, AFKManagerInit){
	afk_manager.init(db);
	EXPECT_EQ("https://forums.alliedmods.net/showpost.php?p=708265", afk_manager.getUrl());
	ASSERT_EQ(2, afk_manager.size());

	EXPECT_EQ('p', afk_manager.getFileTag(0));
	EXPECT_EQ('t', afk_manager.getFileTag(1));

	EXPECT_EQ("plugins", afk_manager.getFileDir(0));
	EXPECT_EQ("translations", afk_manager.getFileDir(1));

	EXPECT_EQ("http://afkmanager.dawgclan.net/plugins/afk_manager4.smx", afk_manager.getFileName(0));
	EXPECT_EQ("http://afkmanager.dawgclan.net/translations/afk_manager.phrases.txt", afk_manager.getFileName(1));
}

TEST_F(PlanTest, AFKManagerFetch){
	afk_manager.init(db);
	AMScraper scraper(downloader);
	afk_manager.fetch(scraper);

	ASSERT_EQ(2, afk_manager.size());
	EXPECT_EQ("http://afkmanager.dawgclan.net/plugins/afk_manager4.smx", afk_manager.getFileUrl(0));
	EXPECT_EQ("http://afkmanager.dawgclan.net/translations/afk_manager.phrases.txt", afk_manager.getFileUrl(1));
}
