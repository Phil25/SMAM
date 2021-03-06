#include "archive.h"

#include <zip.h>
#include <fstream>

namespace smam
{
bool archive::IsValidArchive(const std::filesystem::path& file) noexcept
{
    return file.extension() == ".zip";
}

bool archive::Extract(const std::filesystem::path& zipFile,
                      const EachFile&              callback) noexcept
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

        callback(file);

        zip_file* f = zip_fopen(z, s.name, 0);

        char contents[s.size];
        zip_fread(f, contents, s.size);

        std::ofstream(file).write(contents, s.size);
        zip_fclose(f);
    }

    zip_close(z);
    return true;
}
}  // namespace smam
