#include <operations/addon.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <operations/installer.h>

#include <net/database.h>

#include <scrapers/amscraper.h>
#include <scrapers/ghscraper.h>
#include <scrapers/ltscraper.h>

namespace smam
{
using namespace testing;

class OperationAddonTest : public ::testing::Test
{
protected:
    LoggerPtr   logger = std::make_shared<Logger>();
    const char* url    = "localhost:7666";

    auto GetAddon(const std::string& id) noexcept -> AddonPtr
    {
        const auto cache = Database(logger, url, {id}).Cached();

        if (auto it = cache.find(id); it != cache.end())
        {
            return it->second;
        }

        return {};
    }
};

TEST_F(OperationAddonTest, ContextConstructor)
{
    auto exec = Executor<AddonContext>(logger, GetAddon("rtd"));
    EXPECT_EQ("rtd", exec.GetContext().addon->ID());
}

TEST_F(OperationAddonTest, FindData)
{
    auto addon = GetAddon("rtd");
    auto exec  = Executor<AddonContext>(logger, addon);

    auto scrapers   = std::make_shared<ScraperArray>();
    scrapers->at(0) = std::make_unique<AMScraper>();
    scrapers->at(1) = std::make_unique<LTScraper>();
    scrapers->at(2) = std::make_unique<GHScraper>();

    ASSERT_EQ("https://github.com/Phil25/RTD/", addon->BaseURL());

    auto error = exec.Run<FindData>(scrapers).GetError();

    ASSERT_FALSE(error) << error.message;

    const auto& data = exec.GetContext().data;

    EXPECT_THAT(
        data.nameToLink,
        ElementsAre(Pair("rtd-2.3.1.zip",
                         "https://github.com/Phil25/RTD/releases/"
                         "download/2.3.1/rtd-2.3.1.zip")));

    EXPECT_EQ("https://github.com/Phil25/RTD/", data.url);
    EXPECT_EQ(Scraper::Data::Website::GitHub, data.website);
}
}  // namespace smam
