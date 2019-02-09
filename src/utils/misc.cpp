#include "misc.h"

#include <sstream>

Utils::StringVector Utils::toLines(const std::string& data){
	std::istringstream dataStream(data);
	std::string line;
	std::vector<std::string> lines;

	while(std::getline(dataStream, line))
		lines.push_back(line);

	return lines;
}

bool Utils::isLink(const std::string& str){
	return str.compare(0, 8, "https://") == 0
		|| str.compare(0, 7, "http://") == 0;
}

std::string Utils::extract(cstr& data, cstr& from, cstr& to){
	size_t begin = data.find(from, 0);
	if(begin == (size_t)std::string::npos)
		return "";

	begin += from.size();

	size_t end = data.find(to, begin);
	if(end == (size_t)std::string::npos)
		return "";

	return data.substr(begin, end -begin);
}

size_t Utils::min(size_t a, size_t b){
	return a < b ? a : b;
}
