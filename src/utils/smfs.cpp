#include "smfs.h"

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
		res = cur / "addons/sourcemod/";
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
