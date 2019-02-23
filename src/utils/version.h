#pragma once

#include "misc.h"

namespace Utils
{

	class Version
	{
		std::vector<int> nums;

	public:
		Version(std::string version);
		int compare(const Version& other) const;

		// return string representing biggest version value
		static const std::string& biggest(const StringVector& versions);
	};

}
