#pragma once

#include <curl/curl.h>
#include <string>
#include <vector>

class Database{

public:
	Database();

	virtual std::vector<std::string> fetch(const std::string& id) const;
};
