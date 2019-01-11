#include <string>
#include <vector>
#include <fstream>

#include "gtest/gtest.h"
#include "../src/scrapers/amscraper.h"

class DownloaderMock : public Downloader{
	static std::map<std::string, std::string> dataLink;

public:
	std::string html(cstr& url, cstr& from, cstr& to) override{
		return extract(getData(url), from, to);
	}

	static std::string getData(std::string url){
		auto it = dataLink.find(url);
		return it == dataLink.end() ? "" : readFile(it->second);
	}

	static std::string readFile(std::string name){
		std::ifstream ifs("../test/mockdata/" + name + ".txt");
		std::istreambuf_iterator<char> start(ifs);
		return std::string(start, std::istreambuf_iterator<char>());
	}
};

std::map<std::string, std::string> DownloaderMock::dataLink = {
	{"https://forums.alliedmods.net/showpost.php?p=1754217", "advancedinfiniteammo"},
	{"https://forums.alliedmods.net/showpost.php?p=708265", "afk_manager"},
	{"https://forums.alliedmods.net/showpost.php?p=1387386", "dynamic_motd"},
	{"https://forums.alliedmods.net/showpost.php?p=665771", "funcommandsx"},
	{"https://forums.alliedmods.net/showpost.php?p=1590169", "thriller"},
};

class AMScraperTest : public ::testing::Test{
protected:
	AMScraper* scr;
	DownloaderMock dmock;

	void SetUp() override{
		scr = new AMScraper(dmock);
		scr->getPage("https://forums.alliedmods.net/showpost.php?p=1590169");
	}

	void TearDown() override{
		delete scr;
	}
};

TEST_F(AMScraperTest, Thriller){
	EXPECT_EQ(
		"http://www.sourcemod.net/vbcompiler.php?file_id=128466",
		scr->getFileUrl("thriller.sp", 'p')
	);
	EXPECT_EQ(
		"https://forums.alliedmods.net/attachment.php?attachmentid=133555",
		scr->getFileUrl("thriller.plugin.txt", 'g')
	);
}
