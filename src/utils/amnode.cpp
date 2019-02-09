#include "amnode.h"
#include "misc.h"

std::string AMNode::anchorStart = "attachment.php";
std::string AMNode::badName = "Get Source";

AMNode::AMNode(const xmlpp::Node* node):
	node(node),
	children(retreiveChildren()),
	href(getHref())
{};

std::vector<AMNode> AMNode::retreiveChildren() const{
	std::vector<AMNode> nodes;
	for(const auto ch : node->get_children())
		nodes.push_back(AMNode(ch));
	return nodes;
}

bool AMNode::isValidAnchor() const{
	if(node->get_name() != "a") return false;
	return href.compare(0, anchorStart.length(), anchorStart) == 0;
}

std::string AMNode::getHref() const{
	auto e = dynamic_cast<const xmlpp::Element*>(node);
	return e ? e->get_attribute_value("href") : "";
}

std::string AMNode::getName() const{
	auto t = dynamic_cast<const xmlpp::TextNode*>(node->get_first_child());
	std::string name = t->get_content();

	if(name == badName){
		t = dynamic_cast<const xmlpp::TextNode*>(node->get_next_sibling());
		name = Utils::extract(t->get_content(), " (", " - ");
	}

	return name;
}

std::string AMNode::getId() const{
	return Utils::extract(href, "attachmentid=", "&");
}

const std::vector<AMNode> AMNode::getChildren() const{
	return children;
}
