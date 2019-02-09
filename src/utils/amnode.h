#pragma once

#include <libxml++/libxml++.h>
#include <string>

class AMNode{
	const xmlpp::Node* node;
	const std::vector<AMNode> children;
	const std::string href;

	// defines what href of anchor should start with to be valid
	static std::string anchorStart;

	// specifies what name invalidades this anchor
	static std::string badName;

public:
	AMNode(const xmlpp::Node* node);

private:
	// retreive children of node as vector of this object
	std::vector<AMNode> retreiveChildren() const;

public:
	// is this node an achor and is it valid
	bool isValidAnchor() const;

	// retreive href attribute of anchor
	std::string getHref() const;

	// retreive name of the attachment
	std::string getName() const;

	// retreive id of the attachment
	std::string getId() const;

	// retreive id of the attachment
	const std::vector<AMNode> getChildren() const;
};
