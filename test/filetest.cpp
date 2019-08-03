#include "../src/smfs/file.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <nlohmann/json.hpp>

using namespace testing;
using json = nlohmann::json;

inline auto get(const std::string& file)
{
    return json(file).get<File::Ptr>();
}

TEST(FileTest, DeserializeLocal)
{
    auto file = get("path/to/file.txt");

    EXPECT_EQ("path/to/file.txt", file->raw());
    EXPECT_EQ("file.txt", file->getName());
    EXPECT_EQ("path/to", file->getPath());
    EXPECT_TRUE(file->getUrl().empty());
}

TEST(FileTest, DeserializeRemote)
{
    auto file1 = get("path/to/;file.txt");

    EXPECT_EQ("path/to/file.txt", file1->raw());
    EXPECT_EQ("file.txt", file1->getName());
    EXPECT_EQ("path/to", file1->getPath());
    EXPECT_TRUE(file1->getUrl().empty());

    auto file2 = get("path/to;file.txt");

    EXPECT_EQ("path/to/file.txt", file2->raw());
    EXPECT_EQ("file.txt", file2->getName());
    EXPECT_EQ("path/to", file2->getPath());
    EXPECT_TRUE(file2->getUrl().empty());

    auto file3 = get("path/to/;https://web.com/file.txt");

    EXPECT_EQ("path/to/https://web.com/file.txt", file3->raw());
    EXPECT_EQ("https://web.com/file.txt", file3->getName());
    EXPECT_EQ("path/to", file3->getPath());
    EXPECT_TRUE(file3->getUrl().empty());
}

TEST(FileTest, SerializeLocal)
{
    EXPECT_EQ("path/to/file.txt", get("path/to/file.txt")->raw());
}

TEST(FileTest, PathConversion)
{
    EXPECT_EQ(std::filesystem::path("path/to/file.txt"),
              *get("path/to/file.txt"));
}

TEST(FileTest, Comparison)
{
    auto file1 = get("path/to/fileA.txt");
    auto file2 = get("path/to/fileA.txt");
    auto file3 = get("path/to/fileB.txt");

    EXPECT_EQ(file1, file2);
    EXPECT_NE(file1, file3);
    EXPECT_NE(file2, file3);
}

TEST(FileTest, EvaluateAttachment)
{
    auto plugin   = get("plugins/;plugin.smx");
    auto source   = get("scripting/;plugin.sp");
    auto gamedata = get("gamedata/;gamedata.txt");

    ASSERT_EQ("plugins", plugin->getPath());
    ASSERT_EQ("plugin.smx", plugin->getName());
    ASSERT_EQ("", plugin->getUrl());

    ASSERT_EQ("scripting", source->getPath());
    ASSERT_EQ("plugin.sp", source->getName());
    ASSERT_EQ("", source->getUrl());

    ASSERT_EQ("gamedata", gamedata->getPath());
    ASSERT_EQ("gamedata.txt", gamedata->getName());
    ASSERT_EQ("", gamedata->getUrl());

    auto data            = Scraper::Data{};
    data["plugin.smx"]   = "http://www.down.com/plugin.smx";
    data["plugin.sp"]    = "http://www.down.com/plugin.sp";
    data["gamedata.txt"] = "http://www.down.com/gamedata.txt";

    ASSERT_TRUE(plugin->evaluate(data));
    ASSERT_TRUE(source->evaluate(data));
    ASSERT_TRUE(gamedata->evaluate(data));

    EXPECT_EQ("plugins", plugin->getPath());
    EXPECT_EQ("plugin.smx", plugin->getName());
    EXPECT_EQ("http://www.down.com/plugin.smx", plugin->getUrl());

    EXPECT_EQ("scripting", source->getPath());
    EXPECT_EQ("plugin.sp", source->getName());
    EXPECT_EQ("http://www.down.com/plugin.sp", source->getUrl());

    EXPECT_EQ("gamedata", gamedata->getPath());
    EXPECT_EQ("gamedata.txt", gamedata->getName());
    EXPECT_EQ("http://www.down.com/gamedata.txt", gamedata->getUrl());
}

TEST(FileTest, EvaluateLink)
{
    auto file = get("path/to/;https://web.com/file.txt");

    ASSERT_EQ("path/to", file->getPath());
    ASSERT_EQ("https://web.com/file.txt", file->getName());
    ASSERT_EQ("", file->getUrl());

    ASSERT_TRUE(file->evaluate({}));

    EXPECT_EQ("path/to", file->getPath());
    EXPECT_EQ("file.txt", file->getName());
    EXPECT_EQ("https://web.com/file.txt", file->getUrl());
}

TEST(FileTest, EvaluateRegex)
{
    auto file = get("plugins/;binary-.*.smx");

    ASSERT_EQ("plugins", file->getPath());
    ASSERT_EQ("binary-.*.smx", file->getName());
    ASSERT_EQ("", file->getUrl());

    auto data               = Scraper::Data{};
    data["binary-2.5.smx"]  = "https://download.com/binary-2.5.smx";
    data["binary-2.14.smx"] = "https://download.com/binary-2.14.smx";
    data["binary-2.1.smx"]  = "https://download.com/binary-2.1.smx";

    ASSERT_TRUE(file->evaluate(data));

    EXPECT_EQ("plugins", file->getPath());
    EXPECT_EQ("binary-2.14.smx", file->getName());
    EXPECT_EQ("https://download.com/binary-2.14.smx", file->getUrl());
}

TEST(FileTest, EvaluateRegexComplex)
{
    auto file = get("plugins/;b-.*.smx");

    ASSERT_EQ("plugins", file->getPath());
    ASSERT_EQ("b-.*.smx", file->getName());
    ASSERT_EQ("", file->getUrl());

    auto data               = Scraper::Data{};
    data["b-2.5.6-99.smx"]  = "https://down.com/b-2.5.6-99.smx";
    data["b-2.5.6-104.smx"] = "https://down.com/b-2.5.6-104.smx";
    data["b-2.5.6-20.smx"]  = "https://down.com/b-2.5.6-20.smx";
    data["b-2.1-200.smx"]   = "https://down.com/b-2.1-200.smx";
    data["b-2.5.6-103.smx"] = "https://down.com/b-2.5.6-103.smx";

    ASSERT_TRUE(file->evaluate(data));

    EXPECT_EQ("plugins", file->getPath());
    EXPECT_EQ("b-2.5.6-104.smx", file->getName());
    EXPECT_EQ("https://down.com/b-2.5.6-104.smx", file->getUrl());
}

TEST(FileTest, EvaluateFallbackGeneral)
{
    auto bin = get("plugins;bin.smx");
    auto tr  = get("translations;tr.txt");

    ASSERT_EQ("plugins", bin->getPath());
    ASSERT_EQ("bin.smx", bin->getName());
    ASSERT_EQ("", bin->getUrl());

    ASSERT_EQ("translations", tr->getPath());
    ASSERT_EQ("tr.txt", tr->getName());
    ASSERT_EQ("", tr->getUrl());

    auto data = Scraper::Data{};
    data.url  = "https://down.com/files";

    ASSERT_TRUE(bin->evaluate(data));
    ASSERT_TRUE(tr->evaluate(data));

    EXPECT_EQ("plugins", bin->getPath());
    EXPECT_EQ("bin.smx", bin->getName());
    EXPECT_EQ("https://down.com/files/bin.smx", bin->getUrl());

    EXPECT_EQ("translations", tr->getPath());
    EXPECT_EQ("tr.txt", tr->getName());
    EXPECT_EQ("https://down.com/files/tr.txt", tr->getUrl());
}

TEST(FileTest, EvaluateFallbackAllLinks)
{
    auto bin = get("p/;http://d.com/plugins/bin.smx");
    auto tr  = get("t/;http://d.com/translations/tr.txt");

    ASSERT_EQ("p", bin->getPath());
    ASSERT_EQ("http://d.com/plugins/bin.smx", bin->getName());
    ASSERT_EQ("", bin->getUrl());

    ASSERT_EQ("t", tr->getPath());
    ASSERT_EQ("http://d.com/translations/tr.txt", tr->getName());
    ASSERT_EQ("", tr->getUrl());

    ASSERT_TRUE(bin->evaluate({}));
    ASSERT_TRUE(tr->evaluate({}));

    EXPECT_EQ("p", bin->getPath());
    EXPECT_EQ("bin.smx", bin->getName());
    EXPECT_EQ("http://d.com/plugins/bin.smx", bin->getUrl());

    EXPECT_EQ("t", tr->getPath());
    EXPECT_EQ("tr.txt", tr->getName());
    EXPECT_EQ("http://d.com/translations/tr.txt", tr->getUrl());
}

TEST(FileTest, EvaluateFallbackAlliedModdersEmpty)
{
    auto bin = get("plugins/;bin.smx");

    ASSERT_EQ("plugins", bin->getPath());
    ASSERT_EQ("bin.smx", bin->getName());
    ASSERT_EQ("", bin->getUrl());

    auto data    = Scraper::Data{};
    data.website = Scraper::Data::Website::AlliedModders;

    EXPECT_FALSE(bin->evaluate(data));
}

TEST(FileTest, EvaluateFallbackAlliedModdersNotFound)
{
    auto bin = get("plugins/;bin.smx");

    ASSERT_EQ("plugins", bin->getPath());
    ASSERT_EQ("bin.smx", bin->getName());
    ASSERT_EQ("", bin->getUrl());

    auto data          = Scraper::Data{};
    data["notbin.smx"] = "https://down.com/notbin.smx";
    data.website       = Scraper::Data::Website::AlliedModders;

    EXPECT_FALSE(bin->evaluate(data));
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
