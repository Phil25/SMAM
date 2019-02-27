#pragma once

#include <string>
#include <vector>
#include <tuple>
#include <map>

#include "downloader.h"
#include "utils/file.hpp"

// general url & vector of file names
using Plan = std::tuple<std::string, std::vector<File>>;

class Database
{
	Downloader& downloader;

	Plan nullPlan = {"", {}};
	std::map<std::string, Plan> precached;

public:
	Database(Downloader&);

	// precache data for multiple potential requests
	void precache(const std::vector<std::string>& ids);

	// return plan for particular addon id
	const Plan& get(const std::string& id) const;
};
