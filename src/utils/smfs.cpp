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
 * Load installed addons from `dataFile` file into `data` map
 */
void SMFS::loadData(const fs::path& dataFile)
{
	std::ifstream ifs(dataFile);
	std::string id;
	fs::path file;

	while(ifs >> id >> file) addFile(file, id);
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

/*
 * Add file to specified addon ID's cache
 */
void SMFS::addFile(const fs::path& file, const std::string& id)
{
	data[id].insert(file);
}

/*
 * Erase belonging of a file to an addon. Doesn't remove it from disk
 */
bool SMFS::eraseFile(const fs::path& file, const std::string& id)
{
	if(!isInstalled(id)) return false;

	data[id].erase(file);
	return true;
}

/*
 * Remove a file related to an addon
 */
auto SMFS::removeFile(const fs::path& file) -> DeleteResult
{
	if(!fs::exists(file)) return DeleteResult::NotExists;
	if(countSharedFiles(file) > 1) return DeleteResult::Shared;

	fs::remove(file);
	SMFS::removeEmptyDirs(file);
	return DeleteResult::OK;
}

/*
 * Remove an addon from the local cache
 */
void SMFS::removeAddon(const std::string& id)
{
	data.erase(id);
}

/*
 * Remove empty directories starting from the specified path,
 * iterating into shallower directories. This path should always
 * be (in practice) relative to the SourceMod root directory.
 */
void SMFS::removeEmptyDirs(fs::path p)
{
	while(!p.empty())
	{
		if(fs::is_directory(p) && fs::is_empty(p))
		{
			fs::remove_all(p);
		}

		p = p.parent_path();
	}
}

/*
 * Return whether an addon of the given ID is installed
 */
bool SMFS::isInstalled(const std::string& id)
{
	return data.count(id);
}

/*
 * Iterate over installed addons
 */
void SMFS::getInstalled(const ForAddon& cb)
{
	for(const auto& addon : data) cb(addon.first);
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
