#include <operations/addon.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <operations/installer.h>

#include <scrapers/amscraper.h>
#include <scrapers/ghscraper.h>
#include <scrapers/ltscraper.h>

namespace smam
{
using namespace testing;

class OperationAddonTest : public ::testing::Test
{
protected:
    Logger                     logger;
    Executor<AddonContext>     exec{logger};
    Executor<InstallerContext> installer{logger};

    auto GetPlans() noexcept
    {
        const auto& data  = installer.GetContext().data;
        auto        plans = std::vector<AddonPlan>{};

        std::transform(data.cbegin(), data.cend(),
                       std::back_inserter(plans),
                       [](const auto& entry) { return entry.second; });

        return plans;
    }

    void SetUp() override
    {
        const char* url = "localhost:7666";
        const auto  addons =
            std::vector<std::string>{"accelerator", "rtd", "thriller"};

        const auto error =
            installer.Run<PrecacheAddons>(url, addons).GetError();

        ASSERT_FALSE(error) << "SetUp failed: " << error.message;
        ASSERT_EQ(4, GetPlans().size());
    }
};

TEST_F(OperationAddonTest, InitAddonContext)
{
    auto error =
        exec.Run<InitAddonContext>(GetPlans()[1].addon).GetError();

    ASSERT_FALSE(error) << error.message;
    EXPECT_EQ("rtd", exec.GetContext().addon->ID());
}

TEST_F(OperationAddonTest, FindData)
{
    const auto scrapers = ScraperArray{std::make_unique<AMScraper>(),
                                       std::make_unique<LTScraper>(),
                                       std::make_unique<GHScraper>()};

    const auto url = GetPlans()[1].url;
    ASSERT_EQ("https://github.com/Phil25/RTD/", url);

    auto error =
        exec.Run<FindData>(std::cref(scrapers), url).GetError();

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
