#pragma once

#include <string>
#include <vector>

namespace Utils{
	using cstr = const std::string;

	// extract string from data between `from` and `to`
	std::string extract(cstr& data, cstr& from, cstr& to);

	// return const reference to string representing biggest version value
	cstr& biggestVer(const std::vector<std::string>& versions);

	inline size_t min(size_t, size_t);

	class Version{
		std::vector<int> nums;

	public:
		Version(std::string version);
		int compare(const Version& other) const;
	};
}
