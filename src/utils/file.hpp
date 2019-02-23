#pragma once

#include <string>

struct File
{
	const char tag;
	std::string name, url;

	File(const std::string& entry, const std::string url=""):
		tag(entry[0]),
		name(entry.substr(2)),
		url(url)
	{
	}
};
