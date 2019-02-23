#pragma once

#include <string>
#include <vector>
#include <tuple>

#include "utils/file.hpp"

// general url & vector of file names
using Plan = std::tuple<std::string, std::vector<File>>;

class Database
{
public:
	Database();

	virtual Plan fetch(const std::string& id) const;
};
