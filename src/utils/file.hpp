#pragma once

#include <string>

/*
 * Defines a destination path, filename and the URL from which
 * the file can be downloaded.
 */
struct File
{
	std::string path, name, url;

	File(const std::string& data, const std::string url=""):
		url(url)
	{
		auto splitAt = data.find_first_of(';');
		path = data.substr(0, splitAt);
		name = data.substr(++splitAt);
	}
};
