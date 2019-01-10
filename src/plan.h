#pragma once

#include <string>
#include <vector>

class Plan{
	struct File{
		char tag;
		std::string dir,  // SM directory destination
					name, // name or relative path
					url;  // url to download from

		File(const std::string& data);
		static std::string toDir(char);
	};

	std::string url; // main url of the plan
	std::vector<File> files;

public:
	Plan(char** data, int size);

	const std::string& getUrl() const;

	int size() const;

	char getFileTag(int) const;
	const std::string& getFileDir(int) const;
	const std::string& getFileName(int) const;
	const std::string& getFileUrl(int) const;

	void setFileDir(int, const std::string&);
	void setFileName(int, const std::string&);
	void setFileUrl(int, const std::string&);
};
