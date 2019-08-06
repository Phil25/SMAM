#include <gtest/gtest.h>

#ifdef NDEBUG
#undef NDEBUG  // make downloader use curlmock
#endif

#include "../src/database.h"
#include "../src/download.h"

inline auto getFiles(const std::shared_ptr<Addon>& addon) noexcept
{
    auto files = std::vector<File::Ptr>();

    addon->forEachFile([&files](const auto& file) {
        files.push_back(file);
        return true;
    });

    return files;
}

class DatabaseTest : public ::testing::Test
{
protected:
    Database db;

    DatabaseTest() : db("localhost:7666") {}
};

TEST_F(DatabaseTest, PositiveSingle)
{
    db.precache({"accelerator"});

    auto plan = db.get("accelerator");
    ASSERT_TRUE(std::holds_alternative<Database::Plan>(plan));

    const auto& [url, addon] = std::get<Database::Plan>(plan);

    ASSERT_FALSE(url.empty());
    EXPECT_EQ("https://builds.limetech.io/?p=accelerator", url);

    const auto& files = getFiles(addon);
    ASSERT_EQ(1, files.size());
    EXPECT_EQ("../..", files[0]->getPath());
    EXPECT_EQ(".*", files[0]->getName());
}

TEST_F(DatabaseTest, PositiveMultiple)
{
    db.precache({"accelerator", "thriller"});

    auto plan1 = db.get("accelerator");
    auto plan2 = db.get("thriller");

    ASSERT_TRUE(std::holds_alternative<Database::Plan>(plan1));
    ASSERT_TRUE(std::holds_alternative<Database::Plan>(plan2));

    const auto& [url1, addon1] = std::get<Database::Plan>(plan1);
    const auto& [url2, addon2] = std::get<Database::Plan>(plan2);

    EXPECT_FALSE(url1.empty());
    EXPECT_FALSE(getFiles(addon1).empty());

    EXPECT_FALSE(url2.empty());
    EXPECT_FALSE(getFiles(addon2).empty());
}

TEST_F(DatabaseTest, NegativeSingle)
{
    db.precache({"invalid"});
    auto plan = db.get("invalid");

    EXPECT_FALSE(std::holds_alternative<Database::Plan>(plan));
    ASSERT_TRUE(std::holds_alternative<std::string>(plan));

    EXPECT_EQ("not found", std::get<std::string>(plan));
}

TEST_F(DatabaseTest, NegativeMultiple)
{
    db.precache({"invalid", "alsoinvalid"});

    auto plan1 = db.get("invalid");
    auto plan2 = db.get("alsoinvalid");

    EXPECT_FALSE(std::holds_alternative<Database::Plan>(plan1));
    EXPECT_FALSE(std::holds_alternative<Database::Plan>(plan2));

    ASSERT_TRUE(std::holds_alternative<std::string>(plan1));
    ASSERT_TRUE(std::holds_alternative<std::string>(plan2));

    EXPECT_EQ("not found", std::get<std::string>(plan1));
    EXPECT_EQ("not found", std::get<std::string>(plan2));
}

TEST_F(DatabaseTest, PositiveNegativeMix)
{
    db.precache({"invalid", "accelerator", "badadd"});

    EXPECT_TRUE(std::holds_alternative<std::string>(db.get("invalid")));
    EXPECT_TRUE(std::holds_alternative<std::string>(db.get("badadd")));

    const auto& plan = db.get("accelerator");

    ASSERT_TRUE(std::holds_alternative<Database::Plan>(plan));

    const auto& [url, addon] = std::get<Database::Plan>(plan);
    EXPECT_FALSE(url.empty());
    EXPECT_FALSE(getFiles(addon).empty());
}

TEST_F(DatabaseTest, NotPrecached)
{
    db.precache({"accelerator"});

    auto plan1 = db.get("accelerator");
    auto plan2 = db.get("thriller");

    EXPECT_TRUE(std::holds_alternative<Database::Plan>(plan1));
    EXPECT_FALSE(std::holds_alternative<std::string>(plan1));

    EXPECT_FALSE(std::holds_alternative<Database::Plan>(plan2));
    EXPECT_TRUE(std::holds_alternative<std::string>(plan2));
}

TEST_F(DatabaseTest, BadFileAddon)
{
    db.precache({"badfileaddon"});

    auto plan = db.get("badfileaddon");

    EXPECT_FALSE(std::holds_alternative<Database::Plan>(plan));
    ASSERT_TRUE(std::holds_alternative<std::string>(plan));

    EXPECT_EQ("Invalid file: plugins.thriller.smx",
              std::get<std::string>(plan));
}
