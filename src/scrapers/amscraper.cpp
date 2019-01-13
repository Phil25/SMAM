#include "amscraper.h"
#include "../utils.h"

#include "../downloader.h"

AMScraper::AMScraper(Downloader& downloader):
	Scraper(downloader,
		"https://forums.alliedmods.net",
		"<!-- attachments -->",
		"<!-- / attachments -->"
	)
{}

AMScraper::~AMScraper(){}

std::string AMScraper::getFileUrl(const std::string& name, char tag) const{
	if(Utils::isLink(name)) return name;

	int wcPos = name.find('*');
	if(wcPos != (int)std::string::npos)
		return getFileUrl(getWildcard(name, wcPos), tag);

	for(const std::string& line : contents)
		if(line.find(name) != std::string::npos)
			return buildUrl(getId(line), isForumCompilable(tag, name));

	return "";
}

std::string AMScraper::getWildcard(const std::string& name, int at) const{
	std::string start = name.substr(0, at),
				end = name.substr(at+1, name.size());
	std::vector<std::string> versions;

	for(const std::string& line : contents){
		std::string version = Utils::extract(line, start, end);
		if(!version.empty())
			versions.push_back(version);
	}

	return std::string(start + Utils::biggestVer(versions) + end);
}

std::string AMScraper::buildUrl(int id, bool forumCompilable){
	std::string sId = std::to_string(id);
	return std::string(forumCompilable // is forum-compilable plugin
		? "http://www.sourcemod.net/vbcompiler.php?file_id=" +sId
		: "https://forums.alliedmods.net/attachment.php?attachmentid=" +sId
	);
}

int AMScraper::getId(const std::string& line){
	// ...c&amp;attachmentid=116848&amp;d=1435897597\">Get...
	int pos = line.find("attachmentid=");
		pos = line.find('&', pos);

	int id = 0, dec = 1;
	while(line[--pos] != '='){
		id += (line[pos] -'0') *dec;
		dec *= 10;
	}

	return id;
}

bool AMScraper::isForumCompilable(char tag, const std::string& name){
	return tag == 'p' && isFileSource(name);
}

bool AMScraper::isFileSource(const std::string& name){
	int size = name.size();
	if(size < 3) return false;
	return name[size -3] == '.'
		&& name[size -2] == 's'
		&& name[size -1] == 'p';
}
