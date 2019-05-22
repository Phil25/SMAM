#include "smfs.h"

#include "printer.h"

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
 * Load installed addons from `dataFile` file into `data` map
 */
void SMFS::loadData(const fs::path& dataFile)
{
	std::ifstream ifs(dataFile);
	std::string id;
	fs::path file;

	while(ifs >> id >> file) data[id].insert(file);
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
 * Safely prepare directories for a file of an addon, preveting
 * the file.path to go beyond the allowed directory structure.
 */
static bool prepare(const SMFS::fs::path& path)
{
	if(!SMFS::isPathSafe(path)) return false;
	create_directories(path);
	return true;
}

/*
 * Safely prepare directories for a specific file, and add it to cache
 * under its addon's id.
 */
bool SMFS::regFile(const SMFS::fs::path& file, const std::string& id)
{
	if(!prepare(file.parent_path()))
	{
		out(Ch::Warn) << "Ignoring " << file << cr;
		return false;
	}

	bool exists = SMFS::fs::exists(file);
	out(exists ? Ch::Warn : Ch::Std)
		<< (exists ? "Overwriting " : "") << file << cr;

	data[id].insert(file);
	return true;
}

/*
 * Remove addon from local cache
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
