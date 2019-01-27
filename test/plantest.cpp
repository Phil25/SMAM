#include "gtest/gtest.h"

#include "../src/plan.h"
#include "../src/scrapers/amscraper.h"

#include "downloadermock.h"
#include "databasemock.h"

class PlanTest : public ::testing::Test{
protected:
	DatabaseMock db;
	DownloaderMock downloader;

	Plan thriller;

	void SetUp() override{
		thriller.setId("thriller");
	}
};

TEST_F(PlanTest, Init){
	thriller.init(db);
	EXPECT_EQ("https://forums.alliedmods.net/showpost.php?p=1590169", thriller.getUrl());
	ASSERT_EQ(2, thriller.size());

	EXPECT_EQ('p', thriller.getFileTag(0));
	EXPECT_EQ('g', thriller.getFileTag(1));

	EXPECT_EQ("plugins", thriller.getFileDir(0));
	EXPECT_EQ("gamedata", thriller.getFileDir(1));

	EXPECT_EQ("thriller.sp", thriller.getFileName(0));
	EXPECT_EQ("thriller.plugin.txt", thriller.getFileName(1));

	EXPECT_EQ("", thriller.getFileUrl(0));
	EXPECT_EQ("", thriller.getFileUrl(1));
}

TEST_F(PlanTest, Fetch){
	thriller.init(db);
	AMScraper scraper(downloader);
	thriller.fetch(scraper);

	ASSERT_EQ(2, thriller.size());
	EXPECT_EQ("http://www.sourcemod.net/vbcompiler.php?file_id=128466", thriller.getFileUrl(0));
	EXPECT_EQ("https://forums.alliedmods.net/attachment.php?attachmentid=133555", thriller.getFileUrl(1));
}
