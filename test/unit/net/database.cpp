#include <net/database.h>

#include <gtest/gtest.h>

#ifdef NDEBUG
#undef NDEBUG  // make downloader use curlmock
#endif

namespace smam
{
namespace addon
{
constexpr const char* rtd  = "rtd";
constexpr const char* tf2a = "tf2attributes";
constexpr const char* acce = "accelerator";
constexpr const char* thri = "thriller";
constexpr const char* bad1 = "bad1";
constexpr const char* bad2 = "bad2";
constexpr const char* badf = "badfileaddon";
}  // namespace addon

class DatabaseTest : public ::testing::Test
{
protected:
    const char* url = "localhost:7666";
    LoggerPtr   l   = std::make_shared<Logger>();

    template <typename... Args>
    inline auto Fetch(Args... args)
    {
        return db::Fetch(l, url, {std::forward<Args>(args)...});
    }

    void SetUp() override
    {
        l->SetOutput(false);
    }
};

TEST_F(DatabaseTest, PositiveSingle)
{
    auto cache = Fetch(addon::acce);

    ASSERT_EQ(1, cache->size());
    ASSERT_TRUE(cache->count(addon::acce));

    const auto& addon = cache->at(addon::acce);

    ASSERT_FALSE(addon->BaseURL().empty());
    EXPECT_EQ("https://builds.limetech.io/?p=accelerator",
              addon->BaseURL());

    auto files = addon->Files();
    ASSERT_EQ(1, files.size());
    EXPECT_EQ("../..", files[0]->Path());
    EXPECT_EQ(".*", files[0]->Name());
}

TEST_F(DatabaseTest, PositiveMultiple)
{
    auto cache = Fetch(addon::acce, addon::thri);

    ASSERT_EQ(2, cache->size());
    ASSERT_TRUE(cache->count(addon::acce));
    ASSERT_TRUE(cache->count(addon::thri));

    EXPECT_FALSE(cache->at(addon::acce)->BaseURL().empty());
    EXPECT_FALSE(cache->at(addon::acce)->Files().empty());

    EXPECT_FALSE(cache->at(addon::thri)->BaseURL().empty());
    EXPECT_FALSE(cache->at(addon::thri)->Files().empty());
}

TEST_F(DatabaseTest, NegativeSingle)
{
    EXPECT_TRUE(Fetch(addon::bad1)->empty());
}

TEST_F(DatabaseTest, NegativeMultiple)
{
    EXPECT_TRUE(Fetch(addon::bad1, addon::bad2)->empty());
}

TEST_F(DatabaseTest, PositiveNegativeMix)
{
    auto cache = Fetch(addon::bad1, addon::acce, addon::bad2);

    ASSERT_EQ(1, cache->size());
    ASSERT_FALSE(cache->count(addon::bad1));
    ASSERT_TRUE(cache->count(addon::acce));
    ASSERT_FALSE(cache->count(addon::bad2));

    EXPECT_FALSE(cache->at(addon::acce)->BaseURL().empty());
    EXPECT_FALSE(cache->at(addon::acce)->Files().empty());
}

TEST_F(DatabaseTest, Dependencies)
{
    auto cache = Fetch(addon::rtd);

    ASSERT_EQ(2, cache->size());
    ASSERT_TRUE(cache->count(addon::rtd));
    ASSERT_TRUE(cache->count(addon::tf2a));

    EXPECT_FALSE(cache->at(addon::rtd)->BaseURL().empty());
    EXPECT_FALSE(cache->at(addon::tf2a)->Files().empty());
}

TEST_F(DatabaseTest, NotPrecached)
{
    auto cache = Fetch(addon::acce);

    ASSERT_EQ(1, cache->size());
    ASSERT_TRUE(cache->count(addon::acce));
    ASSERT_FALSE(cache->count(addon::thri));
}

TEST_F(DatabaseTest, BadFileAddon)
{
    ASSERT_TRUE(Fetch(addon::badf)->empty());
}
}  // namespace smam
