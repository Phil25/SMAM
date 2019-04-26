#include <filesystem>

namespace smfs
{
	namespace fs = std::filesystem;

	bool goToSMRoot(const fs::path& dest)
	{
		if(!dest.empty())
		{
			fs::current_path(dest);
		}

		if(fs::is_directory("./plugins"))
		{
			return true; // already at SM root
		}

		for(const auto& prev : {"./sourcemod", "./addons/sourcemod"})
		{
			if(fs::is_directory(prev))
			{
				fs::current_path(prev);
				return true;
			}
		}

		return false;
	}

	void preparePath(const std::string& path)
	{
		fs::create_directories(path);
	}

}
