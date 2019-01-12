#include <string>
#include <vector>
#include <fstream>

#include "gtest/gtest.h"
#include "../src/scrapers/amscraper.h"

class DownloaderMock : public Downloader{
public:
	static std::map<std::string, std::string> dataLink;

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

/*TEST_F(AMScraperTest, AFKManager){ // TODO: return links straight away
	int i = getPluinOffset("afk_manager");
	EXPECT_EQ(
		"http://afkmanager.dawgclan.net/plugins/afk_manager4.smx",
		scrapers[i]->getFileUrl("http://afkmanager.dawgclan.net/plugins/afk_manager4.smx", 'p')
	);
	EXPECT_EQ(
		"http://afkmanager.dawgclan.net/translations/afk_manager.phrases.txt",
		scrapers[i]->getFileUrl("http://afkmanager.dawgclan.net/translations/afk_manager.phrases.txt", 't')
	);
}*/

TEST_F(AMScraperTest, DynamicMotd){
	int i = getPluinOffset("dynamic_motd");
	EXPECT_EQ(
		"https://forums.alliedmods.net/attachment.php?attachmentid=160284",
		scrapers[i]->getFileUrl("dynamic_motd.smx", 'p')
	);
}

/*TEST_F(AMScraperTest, FuncommandsX){ TODO: version wildcard functionality
	int i = getPluinOffset("funcommandsx");
	EXPECT_EQ(
		"https://forums.alliedmods.net/attachment.php?attachmentid=159900",
		scrapers[i]->getFileUrl("funcommandsX_*.zip", 'a')
	);
}*/

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
