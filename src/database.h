#pragma once

#include <string>
#include <vector>
#include <tuple>
#include <map>

#include "downloader.h"
#include "utils/file.hpp"

/*
 * Addon's URL & vector of assoiated Files.
 */
using Plan = std::tuple<std::string, std::vector<File>>;

class Database
{
	Downloader& downloader;
	const std::string dbUrl;

	Plan nullPlan = {"", {}};
	std::map<std::string, Plan> precached;

public:
	Database(Downloader&, const std::string& dbUrl="localhost:7666");

	void precache(const std::vector<std::string>& ids);

	const Plan& get(const std::string& id) const;
};
