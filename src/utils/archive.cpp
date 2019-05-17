#include "archive.h"

#include <fstream>
#include <zip.h>

#include "printer.h"
#include "smfs.h"

bool Archive::valid(const fs::path& file)
{
	return file.extension() == ".zip";
}

auto Archive::extract(const fs::path& zipFile) -> std::vector<fs::path>
{
	struct zip* z;
	struct zip_stat s;

	if((z = zip_open(zipFile.c_str(), 0, NULL)) == NULL)
	{
		return {};
	}

	std::vector<fs::path> files;
	out(Ch::Info) << "Extracting " << zipFile.filename() << "..." << cr;

	for(int i = 0; i < zip_get_num_entries(z, 0); ++i)
	{
		if(zip_stat_index(z, i, 0, &s) != 0) continue;

		auto file = zipFile.parent_path();
		file.append(s.name);

		if(!SMFS::prepare(file.parent_path()))
		{
			out(Ch::Warn) << "Ignoring " << file << cr;
			continue;
		}

		out() << file << cr;

		zip_file* f = zip_fopen(z, s.name, 0);

		char contents[s.size];
		zip_fread(f, contents, s.size);

		std::ofstream(file).write(contents, s.size);
		files.push_back(file);

		zip_fclose(f);
	}

	zip_close(z);
	fs::remove(zipFile);
	return files;
}
