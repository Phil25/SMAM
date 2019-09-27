#include <scrapers/ghscraper.h>

#include "common.hpp"

namespace smam
{
class GHScraperTest : public ::testing::Test
{
protected:
    GHScraper     scraper;
    Scraper::Data expected;
};

TEST_F(GHScraperTest, TF2Attributes)
{
    expected.nameToLink["tf2attributes.smx"] =
        "https://github.com/FlaminSarge/tf2attributes/releases/"
        "download/v1.2.1/tf2attributes.smx";

    expected.nameToLink["tf2attributes_example.smx"] =
        "https://github.com/FlaminSarge/tf2attributes/releases/"
        "download/v1.2.1/tf2attributes_example.smx";

    expected.website = Website::GitHub;

    auto actual =
        scraper.Parse("https://github.com/FlaminSarge/tf2attributes/");

    CompareScraperData(expected, actual);
}
}  // namespace smam
