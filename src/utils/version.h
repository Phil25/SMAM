#pragma once

#include "misc.h"

namespace Utils{
	// return const reference to string representing biggest version value
	cstr& biggestVer(const StringVector& versions);

	class Version{
		std::vector<int> nums;

	public:
		Version(std::string version);
		int compare(const Version& other) const;
	};
}
