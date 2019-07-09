#include <gtest/gtest.h>

#include "curlmock.h"

#include <fstream>

#ifdef _PROJECT_ROOT
#define PROJECT_ROOT _PROJECT_ROOT
#else
#define PROJECT_ROOT ".."
#endif

inline auto vecToStr(const std::vector<char>& vec) noexcept
    -> std::string
{
    return {vec.begin(), vec.end()};
}

inline auto readDatabaseFile(const std::string& name) noexcept
    -> std::string
{
    std::string location =
        PROJECT_ROOT "/test/mockdata/database/" + name + ".json";

    std::ifstream ifs(location, std::ios::in);
    if (!ifs.is_open()) return {};

    std::stringstream buffer;
    buffer << ifs.rdbuf();
    ifs.close();

    return buffer.str();
}

TEST(CurlmockTest, QueryDatabaseSingle)
{
    auto url    = "localhost:7666/?ids=thriller";
    auto actual = vecToStr(CURLMock::queryDatabase(url));

    EXPECT_EQ('[' + readDatabaseFile("thriller") + ']', actual);
}

TEST(CurlmockTest, QueryDatabaseMultiple)
{
    auto url    = "localhost:7666/?ids=thriller,tf2attributes";
    auto actual = vecToStr(CURLMock::queryDatabase(url));

    auto expected = '[' + readDatabaseFile("thriller") + ',' +
                    readDatabaseFile("tf2attributes") + ']';

    EXPECT_EQ(expected, actual);
}

TEST(CurlmockTest, QueryDatabaseUnknown)
{
    auto url = "localhost:7666/?ids=unknown";
    EXPECT_EQ("[]", vecToStr(CURLMock::queryDatabase(url)));
}

TEST(CurlmockTest, QueryDatabaseSingleWithUnknown)
{
    auto url    = "localhost:7666/?ids=unknown,tf2items";
    auto actual = vecToStr(CURLMock::queryDatabase(url));

    EXPECT_EQ('[' + readDatabaseFile("tf2items") + ']', actual);
}

TEST(CurlmockTest, QueryDatabaseMultipleWithUnknown)
{
    auto url    = "localhost:7666/?ids=thriller,unknown,tf2items";
    auto actual = vecToStr(CURLMock::queryDatabase(url));

    auto expected = '[' + readDatabaseFile("thriller") + ',' +
                    readDatabaseFile("tf2items") + ']';

    EXPECT_EQ(expected, actual);
}
