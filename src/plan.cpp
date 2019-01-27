#include "plan.h"

Plan::File::File(const std::string& entry):
	tag(entry[0]),
	dir(toDir(entry[0])),
	name(entry.substr(2))
{}

std::string Plan::File::toDir(char tag){
	switch(tag){
		//case 'a': return "";
		case 'c': return "configs";
		case 'd': return "data";
		case 'e': return "extensions";
		case 'g': return "gamedata";
		case 'p': return "plugins";
		case 's': return "scripting";
		case 't': return "translations";
		//case 'u': return "";
	}
	return "";
}

void Plan::setId(const std::string& id){
	this->id = id;
}

void Plan::init(const Database& db){
	std::vector<std::string> data = db.fetch(id);

	int size = data.size();
	if(!size) return;

	url = data.at(0);
	for(int i = 1; i < size; ++i)
		files.push_back(File(data.at(i)));
}

void Plan::fetch(Scraper& scraper){
	scraper.getPage(getUrl());
	for(File& file : files){
		file.name = scraper.getFileName(file.name);
		file.url = scraper.getFileUrl(file.name, file.tag);
	}
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
