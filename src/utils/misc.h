#pragma once

#include <string>
#include <vector>

namespace Utils
{
	using cstr = const std::string;
	using StringVector = std::vector<std::string>;

	// split string to vector by newlines
	StringVector toLines(const std::string&);

	// returns whether given string is a link
	bool isLink(const std::string&);

	// extract string from data between `from` and `to`
	std::string extract(cstr& data, cstr& from, cstr& to);
}
