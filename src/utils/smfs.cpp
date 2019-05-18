#include "smfs.h"

#include <fstream>
#include <set>
#include <map>

// AddonID -> [files]
std::map<std::string, std::set<SMFS::fs::path>> data;

/*
 * Finds SourceMod root ([...]/mod/addons/sourcemod)
 * from the ./mod directory or above.
 */
auto SMFS::findRoot(const fs::path& st) -> MaybePath
{
	auto cur = fs::canonical(fs::exists(st) ? st : fs::current_path()),
		root = cur.root_path();

	fs::path res;
	while(cur != root)
	{
		res = cur/"addons/sourcemod/";
		if(fs::is_directory(res)) return res;

		cur = cur.parent_path();
	}

	return std::nullopt;
}

/*
 * Safely prepare directories for a file of an addon, preveting
 * the file.path to go beyond the allowed directory structure.
 */
bool SMFS::prepare(const fs::path& path)
{
	if(!isPathSafe(path)) return false;
	create_directories(path);
	return true;
}

/*
 * Make sure the path doesn't escape up to 2 directories
 * before the current path.
 */
bool SMFS::isPathSafe(const fs::path& path)
{
	static const fs::path back = "..";

	int depth = 0;
	for(auto f : path.lexically_normal())
	{
		if(f == back && ++depth > 2) return false;
	}

	return true;
}

/*
 * Load installed addons from `dataFile` file into `data` map
 */
void SMFS::loadData(const fs::path& dataFile)
{
	std::ifstream ifs(dataFile);
	std::string id;
	fs::path file;

	while(ifs >> id >> file) addFile(id, file);
	ifs.close();
}

/*
 * Write loaded data from the `data` map into `dataFile` file
 */
bool SMFS::writeData(const fs::path& dataFile)
{
	std::ofstream ofs(dataFile, std::ios::trunc);
	if(!ofs) return false;

	for(const auto& [addon, files] : data)
	{
		for(const auto& file : files)
		{
			ofs << addon << ' ' << file << '\n';
		}
	}

	return true;
}

void SMFS::addFile(const std::string& id, const fs::path& file)
{
	data[id].insert(file);
}

void SMFS::addFiles(const std::string& id, const PathVector& files)
{
	data[id].insert(files.begin(), files.end());
}

void SMFS::removeAddon(const std::string& id)
{
	data.erase(id);
}

/*
 * Return whether an addon of the given ID is installed
 */
bool SMFS::isInstalled(const std::string& id)
{
	return data.count(id);
}

/*
 * Return set of files associated with an AddonID
 */
auto SMFS::getFiles(const std::string& id) -> std::set<fs::path>
{
	return isInstalled(id) ? data[id] : std::set<fs::path>{};
}

/*
 * Return how many addons share the specified file
 */
int SMFS::countSharedFiles(const fs::path& file)
{
	int count = 0;
	for(const auto& addon : data)
	{
		count += addon.second.count(file);
	}
	return count;
}
