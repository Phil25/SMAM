#include <data/file.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <nlohmann/json.hpp>

namespace smam
{
inline auto get(std::string data)
{
    return nlohmann::json(std::move(data)).get<FilePtr>();
}

TEST(FileTest, DeserializeLocal)
{
    auto file = get("path/to/file.txt");

    EXPECT_EQ("path/to/file.txt", file->Raw());
    EXPECT_EQ("file.txt", file->Name());
    EXPECT_EQ("path/to", file->Path());
    EXPECT_TRUE(file->Link().empty());
}

TEST(FileTest, DeserializeRemote)
{
    auto file1 = get("path/to/;file.txt");

    EXPECT_EQ("path/to/file.txt", file1->Raw());
    EXPECT_EQ("file.txt", file1->Name());
    EXPECT_EQ("path/to", file1->Path());
    EXPECT_TRUE(file1->Link().empty());

    auto file2 = get("path/to;file.txt");

    EXPECT_EQ("path/to/file.txt", file2->Raw());
    EXPECT_EQ("file.txt", file2->Name());
    EXPECT_EQ("path/to", file2->Path());
    EXPECT_TRUE(file2->Link().empty());

    auto file3 = get("path/to/;https://web.com/file.txt");

    EXPECT_EQ("path/to/https://web.com/file.txt", file3->Raw());
    EXPECT_EQ("https://web.com/file.txt", file3->Name());
    EXPECT_EQ("path/to", file3->Path());
    EXPECT_TRUE(file3->Link().empty());
}

TEST(FileTest, SerializeLocal)
{
    EXPECT_EQ("path/to/file.txt", get("path/to/file.txt")->Raw());
}

TEST(FileTest, Comparison)
{
    auto file1 = get("paht/to/fileA.txt");
    auto file2 = get("paht/to/fileA.txt");
    auto file3 = get("paht/to/fileB.txt");

    EXPECT_EQ(file1, file2);
    EXPECT_NE(file1, file3);
    EXPECT_NE(file2, file3);
}

TEST(FileTest, InvalidArgument)
{
    EXPECT_THROW(get(";bin.smx"), std::invalid_argument);
    EXPECT_THROW(get("plugins/;"), std::invalid_argument);
    EXPECT_THROW(get("bin.smx"), std::invalid_argument);
    EXPECT_THROW(get("/bin.smx"), std::invalid_argument);
    EXPECT_THROW(get("plugins/"), std::invalid_argument);
    EXPECT_THROW(get("aa;"), std::invalid_argument);
    EXPECT_THROW(get(";aa"), std::invalid_argument);
    EXPECT_THROW(get(""), std::invalid_argument);
    EXPECT_THROW(get("a/"), std::invalid_argument);
    EXPECT_THROW(get("/a"), std::invalid_argument);
    EXPECT_THROW(get("aa"), std::invalid_argument);
}
}  // namespace smam
