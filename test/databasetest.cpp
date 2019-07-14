#include <gtest/gtest.h>

#ifdef NDEBUG
#undef NDEBUG  // make downloader use curlmock
#endif

#include "../src/database.h"
#include "../src/download.h"

class DatabaseTest : public ::testing::Test
{
protected:
    Database db;

    DatabaseTest() : db("localhost:7666") {}
};

TEST_F(DatabaseTest, PositiveSingle)
{
    db.precache({"accelerator"});

    const auto& [url, data] = db.get("accelerator");

    ASSERT_FALSE(url.empty());
    EXPECT_EQ("https://builds.limetech.io/?p=accelerator", url);

    ASSERT_EQ(1, data.files.size());
    EXPECT_EQ("../../", data.files[0].path);
    EXPECT_EQ(".*", data.files[0].name);
}

TEST_F(DatabaseTest, PositiveMultiple)
{
    db.precache({"accelerator", "thriller"});

    const auto& [url1, data1] = db.get("accelerator");
    EXPECT_FALSE(url1.empty());
    EXPECT_FALSE(data1.files.empty());

    const auto& [url2, data2] = db.get("thriller");
    EXPECT_FALSE(url2.empty());
    EXPECT_FALSE(data2.files.empty());
}

TEST_F(DatabaseTest, NegativeSingle)
{
    db.precache({"invalid"});

    const auto& [url, data] = db.get("invalid");
    EXPECT_TRUE(url.empty());
    EXPECT_TRUE(data.files.empty());
}

TEST_F(DatabaseTest, NegativeMultiple)
{
    db.precache({"invalid", "alsoinvalid"});

    const auto& [url1, data1] = db.get("invalid");
    EXPECT_TRUE(url1.empty());
    EXPECT_TRUE(data1.files.empty());

    const auto& [url2, data2] = db.get("alsoinvalid");
    EXPECT_TRUE(url2.empty());
    EXPECT_TRUE(data2.files.empty());
}

TEST_F(DatabaseTest, PositiveNegativeMix)
{
    db.precache({"invalid", "accelerator", "alsoinvalid"});

    const auto& [url1, data1] = db.get("invalid");
    EXPECT_TRUE(url1.empty());
    EXPECT_TRUE(data1.files.empty());

    const auto& [url2, data2] = db.get("accelerator");
    EXPECT_FALSE(url2.empty());
    EXPECT_FALSE(data2.files.empty());

    const auto& [url3, data3] = db.get("alsoinvalid");
    EXPECT_TRUE(url3.empty());
    EXPECT_TRUE(data3.files.empty());
}

TEST_F(DatabaseTest, NotPrecached)
{
    db.precache({"accelerator"});

    const auto& [url1, data1] = db.get("accelerator");
    EXPECT_FALSE(url1.empty());
    EXPECT_FALSE(data1.files.empty());

    const auto& [url2, data2] = db.get("thriller");
    EXPECT_TRUE(url2.empty());
    EXPECT_TRUE(data2.files.empty());
}
