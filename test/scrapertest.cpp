#include "gtest/gtest.h"
#include "../src/scrapers/scraper.hpp"

TEST(ScraperTest, LineConversion){
	std::vector<std::string> vec = Scraper::toLines("Line1\nLine2\nLine3");
	ASSERT_EQ(3, vec.size());
	EXPECT_EQ("Line1", vec.at(0));
	EXPECT_EQ("Line2", vec.at(1));
	EXPECT_EQ("Line3", vec.at(2));
}

TEST(ScraperTest, LinkRecognition){
	EXPECT_TRUE(Scraper::isLink("https://sth"));
	EXPECT_TRUE(Scraper::isLink("http://sth"));

	EXPECT_FALSE(Scraper::isLink("https:/sth"));
	EXPECT_FALSE(Scraper::isLink("http:/sth"));

	EXPECT_FALSE(Scraper::isLink("ttps://sth"));
	EXPECT_FALSE(Scraper::isLink("ttp://sth"));

	EXPECT_FALSE(Scraper::isLink(" https://sth"));
	EXPECT_FALSE(Scraper::isLink(" http://sth"));

	EXPECT_FALSE(Scraper::isLink("ahttps://sth"));
	EXPECT_FALSE(Scraper::isLink("ahttp://sth"));
}
