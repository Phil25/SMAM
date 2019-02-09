#include "amscraper.h"
#include "../utils/version.h"
#include "../utils/amnode.h"

// build url for downloading attachment
static std::string buildUrl(const std::string& id, bool forumCompilable);

// check if the file is a forum-compilable source
static bool isForumCompilable(char tag, const std::string& name);

// check if name ends with ".sp"
static bool isFileSource(const std::string& name);

// parse attachments from the part of retreived AlliedMods thread
static Attachments parseAttachments(const xmlpp::DomParser&);

// populate vector of attachments with children of root recursively
static void populateAttachments(const AMNode& root, Attachments&);

AMScraper::AMScraper(Downloader& downloader):
	Scraper(downloader,
		"https://forums.alliedmods.net",
		"<!-- attachments -->",
		"<!-- / attachments -->"
	)
{}

AMScraper::~AMScraper(){}

void AMScraper::fetch(const std::string& url){
	xmlpp::DomParser contents;
	try{
		contents.parse_memory(downloader.html(url, dataFrom, dataTo));
		if(contents) attachments = parseAttachments(contents);
	}catch(const std::exception& e){}
}

std::string AMScraper::getFileName(const std::string& name) const{
	int wcPos = name.find('*');
	return wcPos == (int)std::string::npos
		? name : getWildcard(name, wcPos);
}

std::string AMScraper::getFileUrl(const std::string& name, char tag) const{
	if(Utils::isLink(name)) return name;

	auto it = attachments.find(name);
	if(it == attachments.end()) return "";

	return buildUrl(it->second, isForumCompilable(tag, name));
}

std::string AMScraper::getWildcard(const std::string& name, int at) const{
	std::string start = name.substr(0, at),
				end = name.substr(at+1, name.size());
	std::vector<std::string> versions;

	for(const auto& entry : attachments){
		std::string version = Utils::extract(entry.first, start, end);
		if(!version.empty())
			versions.push_back(version);
	}

	return std::string(start + Utils::biggestVer(versions) + end);
}

std::string buildUrl(const std::string& id, bool forumCompilable){
	return forumCompilable ? // source code that compiles on forums
		"http://www.sourcemod.net/vbcompiler.php?file_id=" +id :
		"https://forums.alliedmods.net/attachment.php?attachmentid=" +id;
}

bool isForumCompilable(char tag, const std::string& name){
	return tag == 'p' && isFileSource(name);
}

bool isFileSource(const std::string& name){
	int size = name.size();
	if(size < 3) return false;
	return name[size -3] == '.'
		&& name[size -2] == 's'
		&& name[size -1] == 'p';
}

Attachments parseAttachments(const xmlpp::DomParser& parser){
	const AMNode root = AMNode(parser.get_document()->get_root_node());
	Attachments attachments;
	populateAttachments(root, attachments); // recursive
	return attachments;
}

void populateAttachments(const AMNode& node, Attachments& map){
	if(node.isValidAnchor())
		map.insert(std::make_pair(node.getName(), node.getId()));
	else for(const auto& child : node.getChildren())
		populateAttachments(child, map);
}
