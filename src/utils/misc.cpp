#include "misc.h"

#include <sstream>

Utils::StringVector Utils::toLines(const std::string& data)
{
	std::istringstream dataStream(data);
	std::string line;
	std::vector<std::string> lines;

	while(std::getline(dataStream, line))
	{
		lines.push_back(line);
	}

	return lines;
}

bool Utils::isLink(const std::string& str)
{
	constexpr std::string_view https = "https://";
	constexpr std::string_view http = "http://";

	return str.compare(0, https.size(), https) == 0
		|| str.compare(0, http.size(), http) == 0;
}

std::string Utils::extract(cstr& s, cstr& from, cstr& to)
{
	if(from.empty() || to.empty()) return s;

	size_t start = s.find(from) + from.size();
	size_t end = s.find(to, start);

	return s.substr(start, end -start);
}
