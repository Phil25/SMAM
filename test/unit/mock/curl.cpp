#include <mock/curl.h>

#include <gtest/gtest.h>

#include <fstream>
#include <nlohmann/json.hpp>

#ifdef _PROJECT_ROOT
#define PROJECT_ROOT _PROJECT_ROOT
#else
#define PROJECT_ROOT ".."
#endif

using json = nlohmann::json;

namespace
{
void ReadDatabaseFile(json& j, const std::string& id) noexcept
{
    auto location =
        PROJECT_ROOT "/test/mock/data/database/" + id + ".json";

    std::ifstream ifs(location, std::ios::in);
    if (!ifs.is_open()) return;

    j.push_back(json::parse(ifs));
    ifs.close();
}

auto ToJSON(const std::vector<std::string>& ids) noexcept
{
    auto result = json::array();

    for (const auto& id : ids)
    {
        ReadDatabaseFile(result, id);
    }

    return result;
}

inline void Check(const std::string&              url,
                  const std::vector<std::string>& v) noexcept
{
    EXPECT_EQ(ToJSON(v), json::parse(mockcurl::QueryDatabase(url)));
}

}  // namespace

namespace nlohmann
{
// Provide string checking method for Google Test to use.
inline void PrintTo(json const& json, std::ostream* os)
{
    *os << json.dump();
}
}  // namespace nlohmann

TEST(CurlmockTest, QueryDatabaseSingle)
{
    Check("localhost:7666/?ids=thriller", {"thriller"});
}

TEST(CurlmockTest, QueryDatabaseMultiple)
{
    Check("localhost:7666/?ids=thriller,tf2attributes",
          {"thriller", "tf2attributes"});
}

TEST(CurlmockTest, QueryDatabaseUnknown)
{
    Check("localhost:7666/?ids=unknown", {});
}

TEST(CurlmockTest, QueryDatabaseSingleWithUnknown)
{
    Check("localhost:7666/?ids=unknown,tf2items", {"tf2items"});
}

TEST(CurlmockTest, QueryDatabaseMultipleWithUnknown)
{
    Check("localhost:7666/?ids=thriller,unknown,tf2items",
          {"thriller", "tf2items"});
}

TEST(CurlmockTest, QueryDatabaseDependency)
{
    Check("localhost:7666/?ids=rtd", {"rtd", "tf2attributes"});
}

TEST(CurlmockTest, QueryDatabaseDependencyExplicit)
{
    Check("localhost:7666/?ids=tf2attributes,rtd",
          {"tf2attributes", "rtd"});
}

TEST(CurlmockTest, QueryDatabaseMultipleWithDependency)
{
    Check("localhost:7666/?ids=rtd,tf2items",
          {"rtd", "tf2attributes", "tf2items"});
}

TEST(CurlmockTest, QueryDatabaseMultipleWithDependencyExplicit)
{
    Check("localhost:7666/?ids=tf2attributes,rtd,tf2items",
          {"tf2attributes", "rtd", "tf2items"});
}

TEST(CurlmockTest, QueryDatabaseMultipleWithDependencyRepeats)
{
    Check("localhost:7666/?ids=rtd,tf2items,tf2items,rtd",
          {"rtd", "tf2attributes", "tf2items"});
}

TEST(CurlmockTest, QueryDatabaseCircularDependency)
{
    Check("localhost:7666/?ids=a_wants_b", {"a_wants_b", "b_wants_a"});
}

TEST(CurlmockTest, QueryDatabaseCircularDependencyExplicit)
{
    Check("localhost:7666/?ids=b_wants_a,a_wants_b",
          {"b_wants_a", "a_wants_b"});
}

TEST(CurlmockTest, QueryDatabaseCircularDependencyRepeats)
{
    Check("localhost:7666/?ids=a_wants_b,a_wants_b",
          {"a_wants_b", "b_wants_a"});
}

TEST(CurlmockTest, QueryDatabasePluginPack)
{
    Check("localhost:7666/?ids=pluginpack",
          {"pluginpack", "thriller", "sharedthriller"});
}

TEST(CurlmockTest, QueryDatabasePluginPackExplicitSingle)
{
    Check("localhost:7666/?ids=thriller,pluginpack",
          {"thriller", "pluginpack", "sharedthriller"});
}

TEST(CurlmockTest, QueryDatabasePluginPackExplicitMultiple)
{
    Check("localhost:7666/?ids=sharedthriller,thriller,pluginpack",
          {"sharedthriller", "thriller", "pluginpack"});
}

TEST(CurlmockTest, QueryDatabasePluginPackCircularDependency)
{
    Check("localhost:7666/?ids=pluginpack_circular",
          {"pluginpack_circular", "a_wants_b", "b_wants_a"});
}

TEST(CurlmockTest,
     QueryDatabasePluginPackCircularDependencyExplicitSingle)
{
    Check("localhost:7666/?ids=a_wants_b,pluginpack_circular",
          {"a_wants_b", "b_wants_a", "pluginpack_circular"});
}

TEST(CurlmockTest,
     QueryDatabasePluginPackCircularDependencyExplicitMultiple)
{
    Check("localhost:7666/?ids=b_wants_a,a_wants_b,pluginpack_circular",
          {"b_wants_a", "a_wants_b", "pluginpack_circular"});
}

TEST(CurlmockTest, QueryDatabasePluginPackDependencyHierarchy)
{
    Check("localhost:7666/?ids=pluginpack_hierarchy",
          {"pluginpack_hierarchy", "rtd", "tf2attributes"});
}

TEST(CurlmockTest, QueryDatabasePluginPackDependencyHierarchyExplicit)
{
    Check("localhost:7666/?ids=tf2attributes,pluginpack_hierarchy",
          {"tf2attributes", "pluginpack_hierarchy", "rtd"});
}
