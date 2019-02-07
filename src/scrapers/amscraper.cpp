#include <libxml++/libxml++.h>

#include "amscraper.h"
#include "../utils.h"

// wrapper for xmlpp::Node* management
struct Node{
	const xmlpp::Node* node;
	const std::vector<Node> children;
	const std::string href;

	// defines what href of anchor should start with to be valid
	static std::string anchorStart;

	// specifies what name invalidades this anchor
	static std::string badName;

	Node(const xmlpp::Node* node);

	// retreive children of node as vector of this object
	std::vector<Node> retreiveChildren() const;

	// populate vector of attachments with children recursively
	void populate(Attachments&) const;

	// is this node an achor and is it valid
	bool isValidAnchor() const;

	// retreive href attribute of anchor
	std::string getHref() const;

	// retreive name of the attachment
	std::string getName() const;

	// retreive id of the attachment
	std::string getId() const;
};

// parse attachments from the part of retreived AlliedMods thread
static Attachments parseAttachments(const xmlpp::DomParser&);

// build url for downloading attachment
static std::string buildUrl(const std::string& id, bool forumCompilable);

// check if the file is a forum-compilable source
static bool isForumCompilable(char tag, const std::string& name);

// check if name ends with ".sp"
static bool isFileSource(const std::string& name);

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
	}catch(const std::exception& e){}
	if(contents) attachments = parseAttachments(contents);
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

Attachments parseAttachments(const xmlpp::DomParser& parser){
	const Node root = Node(parser.get_document()->get_root_node());
	Attachments attachments;
	root.populate(attachments); // recursive
	return attachments;
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

// Node wrapper definitions

std::string Node::anchorStart = "attachment.php";
std::string Node::badName = "Get Source";

Node::Node(const xmlpp::Node* node):
	node(node),
	children(retreiveChildren()),
	href(getHref())
{};

std::vector<Node> Node::retreiveChildren() const{
	std::vector<Node> nodes;
	for(const auto ch : node->get_children())
		nodes.push_back(Node(ch));
	return nodes;
}

void Node::populate(Attachments& map) const{
	if(isValidAnchor())
		map.insert(std::make_pair(getName(), getId()));
	for(const auto child : children)
		child.populate(map);
}

bool Node::isValidAnchor() const{
	if(node->get_name() != "a") return false;
	return href.compare(0, anchorStart.length(), anchorStart) == 0;
}

std::string Node::getHref() const{
	auto e = dynamic_cast<const xmlpp::Element*>(node);
	return e ? e->get_attribute_value("href") : "";
}

std::string Node::getName() const{
	auto t = dynamic_cast<const xmlpp::TextNode*>(node->get_first_child());
	std::string name = t->get_content();

	if(name == badName){
		t = dynamic_cast<const xmlpp::TextNode*>(node->get_next_sibling());
		name = Utils::extract(t->get_content(), " (", " - ");
	}

	return name;
}

std::string Node::getId() const{
	return Utils::extract(href, "attachmentid=", "&");
}
