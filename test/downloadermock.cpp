#include "downloadermock.h"

#include "../src/utils.h"

using cstr = const std::string;

std::string DownloaderMock::html(cstr& url, cstr& from, cstr& to){
	return Utils::extract(getData(url), from, to);
}

std::string DownloaderMock::getData(std::string url){
	auto it = dataLink.find(url);
	return it == dataLink.end() ? "" : readFile(it->second);
}

std::string DownloaderMock::readFile(std::string name){
	std::ifstream ifs("../test/mockdata/" + name + ".txt");
	std::istreambuf_iterator<char> start(ifs);
	return std::string(start, std::istreambuf_iterator<char>());
}

std::map<std::string, std::string> DownloaderMock::dataLink = {
	{"https://forums.alliedmods.net/showpost.php?p=1754217", "advancedinfiniteammo"},
	{"https://forums.alliedmods.net/showpost.php?p=708265", "afk_manager"},
	{"https://forums.alliedmods.net/showpost.php?p=1387386", "dynamic_motd"},
	{"https://forums.alliedmods.net/showpost.php?p=665771", "funcommandsx"},
	{"https://forums.alliedmods.net/showpost.php?p=1590169", "thriller"},
};