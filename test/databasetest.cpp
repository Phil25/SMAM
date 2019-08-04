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
    ASSERT_TRUE(plan.has_value());

    const auto& [url, addon] = plan.value();

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

    ASSERT_TRUE(plan1.has_value());
    ASSERT_TRUE(plan2.has_value());

    const auto& [url1, addon1] = plan1.value();
    const auto& [url2, addon2] = plan2.value();

    EXPECT_FALSE(url1.empty());
    EXPECT_FALSE(getFiles(addon1).empty());

    EXPECT_FALSE(url2.empty());
    EXPECT_FALSE(getFiles(addon2).empty());
}

TEST_F(DatabaseTest, NegativeSingle)
{
    db.precache({"invalid"});
    EXPECT_FALSE(db.get("invalid").has_value());
}

TEST_F(DatabaseTest, NegativeMultiple)
{
    db.precache({"invalid", "alsoinvalid"});
    EXPECT_FALSE(db.get("invalid").has_value());
    EXPECT_FALSE(db.get("alsoinvalid").has_value());
}

TEST_F(DatabaseTest, PositiveNegativeMix)
{
    db.precache({"invalid", "accelerator", "alsoinvalid"});

    EXPECT_FALSE(db.get("invalid").has_value());
    EXPECT_FALSE(db.get("alsoinvalid").has_value());

    const auto& plan = db.get("accelerator");
    ASSERT_TRUE(plan.has_value());

    const auto& [url, addon] = plan.value();
    EXPECT_FALSE(url.empty());
    EXPECT_FALSE(getFiles(addon).empty());
}

TEST_F(DatabaseTest, NotPrecached)
{
    db.precache({"accelerator"});
    EXPECT_TRUE(db.get("accelerator").has_value());
    EXPECT_FALSE(db.get("thriller").has_value());
}
