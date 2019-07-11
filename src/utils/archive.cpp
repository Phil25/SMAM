#include "archive.h"

#include <fstream>

#include <zip.h>

bool Archive::valid(const fs::path& file) noexcept
{
    return file.extension() == ".zip";
}

bool Archive::extract(const fs::path& zipFile,
                      const FileCb&   cb) noexcept
{
    struct zip*     z;
    struct zip_stat s;

    if ((z = zip_open(zipFile.c_str(), 0, NULL)) == NULL)
    {
        return false;
    }

    for (int i = 0; i < zip_get_num_entries(z, 0); ++i)
    {
        if (zip_stat_index(z, i, 0, &s) != 0) continue;

        auto file = zipFile.parent_path();
        file.append(s.name);

        if (file.filename().empty()) continue;

        if (!cb(file)) continue;

        zip_file* f = zip_fopen(z, s.name, 0);

        char contents[s.size];
        zip_fread(f, contents, s.size);

        std::ofstream(file).write(contents, s.size);
        zip_fclose(f);
    }

    zip_close(z);
    fs::remove(zipFile);
    return true;
}
