#include "gtest/gtest.h"

#ifdef NDEBUG
#undef NDEBUG // make downloader use curlmock
#endif

#include "../src/database.h"

using FileVector = const std::vector<File>;

class DatabaseTest : public ::testing::Test
{
protected:
	Downloader downloader;
	Database db;

	DatabaseTest():
		db(downloader)
	{
	}
};

TEST_F(DatabaseTest, PositiveSingle)
{
	db.precache({"accelerator"});

	const auto& [url, files] = db.get("accelerator");

	ASSERT_FALSE(url.empty());
	EXPECT_EQ("https://builds.limetech.io/?p=accelerator", url);

	ASSERT_EQ(1, files.size());
	EXPECT_EQ('a', files[0].tag);
	EXPECT_EQ(".*", files[0].name);
}

TEST_F(DatabaseTest, PositiveMultiple)
{
	db.precache({"accelerator", "thriller"});

	const auto& [url1, files1] = db.get("accelerator");
	EXPECT_FALSE(url1.empty());
	EXPECT_FALSE(files1.empty());

	const auto& [url2, files2] = db.get("thriller");
	EXPECT_FALSE(url2.empty());
	EXPECT_FALSE(files2.empty());
}

TEST_F(DatabaseTest, NegativeSingle)
{
	db.precache({"invalid"});

	const auto& [url, files] = db.get("invalid");
	EXPECT_TRUE(url.empty());
	EXPECT_TRUE(files.empty());
}

TEST_F(DatabaseTest, NegativeMultiple)
{
	db.precache({"invalid", "alsoinvalid"});

	const auto& [url1, files1] = db.get("invalid");
	EXPECT_TRUE(url1.empty());
	EXPECT_TRUE(files1.empty());

	const auto& [url2, files2] = db.get("alsoinvalid");
	EXPECT_TRUE(url2.empty());
	EXPECT_TRUE(files2.empty());
}

TEST_F(DatabaseTest, PositiveNegativeMix)
{
	db.precache({"invalid", "accelerator", "alsoinvalid"});

	const auto& [url1, files1] = db.get("invalid");
	EXPECT_TRUE(url1.empty());
	EXPECT_TRUE(files1.empty());

	const auto& [url2, files2] = db.get("accelerator");
	EXPECT_FALSE(url2.empty());
	EXPECT_FALSE(files2.empty());

	const auto& [url3, files3] = db.get("alsoinvalid");
	EXPECT_TRUE(url3.empty());
	EXPECT_TRUE(files3.empty());
}

TEST_F(DatabaseTest, NotPrecached)
{
	db.precache({"accelerator"});

	const auto& [url1, files1] = db.get("accelerator");
	EXPECT_FALSE(url1.empty());
	EXPECT_FALSE(files1.empty());

	const auto& [url2, files2] = db.get("thriller");
	EXPECT_TRUE(url2.empty());
	EXPECT_TRUE(files2.empty());
}
