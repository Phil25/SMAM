#include "plan.h"

Plan::File::File(const std::string& entry):
	tag(entry[0]),
	dir(toDir(entry[0])),
	name(entry.substr(2)),
	url("about:blank")
{}

std::string Plan::File::toDir(char tag){
	switch(tag){
		//case 'a': return "tmp/";
		case 'c': return "tmp/configs/";
		case 'd': return "tmp/data/";
		case 'e': return "tmp/extensions/";
		case 'g': return "tmp/gamedata/";
		case 'p': return "tmp/plugins/";
		case 's': return "tmp/scripting/";
		case 't': return "tmp/translations/";
		//case 'u': return "tmp/";
	}
	return "tmp/";
}

Plan::Plan() : url(""){}

Plan::Plan(const std::vector<std::string>& data){
	init(data);
}

void Plan::init(const std::vector<std::string>& data){
	url = data.at(0);
	int size = data.size();
	for(int i = 1; i < size; ++i)
		files.push_back(File(data.at(i)));
}

void Plan::fill(const Scraper& scraper){
	for(File& file : files)
		file.url = scraper.getFileUrl(file.name, file.tag);
}

int Plan::size() const{
	return files.size();
}

const std::string& Plan::getUrl() const{
	return url;
}

char Plan::getFileTag(int i) const{
	return files.at(i).tag;
}

const std::string& Plan::getFileDir(int i) const{
	return files.at(i).dir;
}

const std::string& Plan::getFileName(int i) const{
	return files.at(i).name;
}

const std::string& Plan::getFileUrl(int i) const{
	return files.at(i).url;
}

void Plan::setFileDir(int i, const std::string& dir){
	files.at(i).dir = dir;
}

void Plan::setFileName(int i, const std::string& name){
	files.at(i).name = name;
}

void Plan::setFileUrl(int i, const std::string& url){
	files.at(i).url = url;
}
