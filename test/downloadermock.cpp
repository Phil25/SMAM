#include <sstream>

#include "downloadermock.h"

#include "../src/utils/misc.h"

using cstr = const std::string;

// map linking url to file name containing the data
extern std::map<std::string, std::string> dataLink;

std::string readFile(const std::string& path)
{
	std::ifstream ifs(path);
	std::istreambuf_iterator<char> start(ifs);

	return std::string(start, std::istreambuf_iterator<char>());
}

std::string readDb(const std::string& url)
{
	size_t pos = url.find('=');

	if(pos == (size_t)std::string::npos)
	{
		return "";
	}

	std::istringstream dataStream(url.substr(++pos));
	std::string data = "[", id;

	while(std::getline(dataStream, id, ','))
	{
		std::string addonData = readFile("../test/dbdata/" + id + ".json");

		if(!addonData.empty())
		{
			data.append(addonData + ',');
		}
	}

	data[data.size() -1] = ']';
	return data;
}

std::string getData(const std::string& url)
{
	auto it = dataLink.find(url);
	if(it == dataLink.end()) // assume it's DB request
	{
		return readDb(url);
	}
	else
	{
		return readFile("../test/mockdata/" + it->second + ".txt");
	}
}

std::string DownloaderMock::html(cstr& url, cstr& from, cstr& to)
{
	return Utils::extract(getData(url), from, to);
}

std::map<std::string, std::string> dataLink = {
	{"https://builds.limetech.io/?p=accelerator", "accelerator"},
	{"https://forums.alliedmods.net/showpost.php?p=1754217", "advancedinfiniteammo"},
	{"https://forums.alliedmods.net/showpost.php?p=708265", "afk_manager"},
	{"https://builds.limetech.io/?p=connect", "connect"},
	{"https://forums.alliedmods.net/showpost.php?p=1387386", "dynamic_motd"},
	{"https://forums.alliedmods.net/showpost.php?p=665771", "funcommandsx"},
	{"https://builds.limetech.io/?p=steamtools", "steamtools"},
	{"https://builds.limetech.io/?p=tf2items", "tf2items"},
	{"https://api.github.com/repos/FlaminSarge/tf2attributes/releases/latest", "tf2attributes"},
	{"https://forums.alliedmods.net/showpost.php?p=1590169", "thriller"},
};
