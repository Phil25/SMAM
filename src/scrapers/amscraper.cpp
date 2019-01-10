#include <sstream>
#include "amscraper.h"

#if defined TEST_BUILD
#include "../../test/mocks/mock-downloader.hpp"
#else
#include "../downloader.h"
#endif

AMScraper::AMScraper():
	Scraper("https://forums.alliedmods.net")
{}

void AMScraper::completePlan(Plan& plan) const{
	std::string from = "<!-- attachments -->",
				to = "<!-- / attachments -->",
				data = Downloader::html(plan.getUrl(), from, to);

	StringVector lines = toLines(data);
	std::string destUrl;
	int size = plan.size();

	for(int i = 0; i < size; ++i){
		destUrl = getUrl(plan.getFileName(i), plan.getFileTag(i), lines);
		plan.setFileUrl(i, destUrl);
	}
}

StringVector AMScraper::toLines(const std::string& data) const{
	std::istringstream dataStream(data);
	std::string line;
	std::vector<std::string> lines;

	while(std::getline(dataStream, line))
		lines.push_back(line);

	return lines;
}

std::string AMScraper::getUrl(
	const std::string& name,
	char tag,
	const StringVector& lines) const
{
	for(auto& line : lines)
		if(line.find(name) != std::string::npos)
			return buildUrl(getId(line), isForumCompilable(tag, name));

	return "";
}

std::string AMScraper::buildUrl(int id, bool forumCompilable) const{
	std::string sId = std::to_string(id);
	return std::string(forumCompilable // is forum-compilable plugin
		? "http://www.sourcemod.net/vbcompiler.php?file_id=" + sId
		: "https://forums.alliedmods.net/attachment.php?attachmentid=" + sId
	);
}

int AMScraper::getId(const std::string& line) const{
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

bool AMScraper::isForumCompilable(char tag, const std::string& name) const{
	return tag == 'p' && isFileSource(name);
}

bool AMScraper::isFileSource(const std::string& name) const{
	int size = name.size();
	if(size < 3) return false;
	return name[size -3] == '.'
		&& name[size -2] == 's'
		&& name[size -1] == 'p';
}
