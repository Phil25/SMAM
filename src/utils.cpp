#include "utils.h"

#include <cctype>
#include <sstream>

// extract string from data between `from` and `to`
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

// return const reference to string representing biggest version value
Utils::cstr& Utils::biggestVer(const std::vector<std::string>& versions){
	int size = versions.size();
	if(size < 2) return versions[0];

	int biggestIndex = 0;
	Version biggestVer = Version(versions[0]);

	for(int i = 1; i < size; ++i){
		Version ver = Version(versions[i]);
		if(ver.compare(biggestVer) == 1){
			biggestVer = ver;
			biggestIndex = i;
		}
	}

	return versions[biggestIndex];
}

inline size_t Utils::min(size_t a, size_t b){
	return a < b ? a : b;
}

// return comparable value of version string, given max number length
Utils::Version::Version(std::string version):
	nums({})
{
	// turn every non-digit word to a space
	for(char& c : version)
		if(!std::isdigit(c))
			c = ' ';

	std::istringstream iss(version);
	std::string word;

	// parse every word (space separated) into the int vector
	while(iss >> word)
		nums.push_back(std::stoi(word));
}

int Utils::Version::compare(const Version& other) const{
	int len = min(nums.size(), other.nums.size());
	for(int i = 0; i < len; ++i){
		if(nums[i] > other.nums[i])
			return 1;
		if(nums[i] < other.nums[i])
			return -1;
	}
	return 0;
}
