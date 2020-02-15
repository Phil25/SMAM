#include <scrapers/ltscraper.h>

#include "common.hpp"

namespace smam
{
class LTScraperTest : public ::testing::Test
{
protected:
    LTScraper     scraper;
    Scraper::Data expected;
};

TEST_F(LTScraperTest, Accelerator)
{
    expected.nameToLink["accelerator-2.4.3-git127-b302f00-linux.zip"] =
        "https://builds.limetech.io/files/"
        "accelerator-2.4.3-git127-b302f00-linux.zip";

    expected.website = Website::Limetech;

    auto actual =
        scraper.Parse("https://builds.limetech.io/?p=accelerator");

    CompareScraperData(expected, actual);
}

TEST_F(LTScraperTest, Connect)
{
    expected.nameToLink["connect-1.2.0-hg38-linux.zip"] =
        "https://builds.limetech.io/files/"
        "connect-1.2.0-hg38-linux.zip";

    expected.website = Website::Limetech;

    auto actual =
        scraper.Parse("https://builds.limetech.io/?p=connect");

    CompareScraperData(expected, actual);
}

TEST_F(LTScraperTest, SteamTools)
{
    expected.nameToLink["steamtools-0.10.0-git179-54fdc51-linux.zip"] =
        "https://builds.limetech.io/files/"
        "steamtools-0.10.0-git179-54fdc51-linux.zip";

    expected.website = Website::Limetech;

    auto actual =
        scraper.Parse("https://builds.limetech.io/?p=steamtools");

    CompareScraperData(expected, actual);
}

TEST_F(LTScraperTest, TF2Items)
{
    expected.nameToLink["tf2items-1.6.4-hg279-linux.zip"] =
        "https://builds.limetech.io/files/"
        "tf2items-1.6.4-hg279-linux.zip";

    expected.website = Website::Limetech;

    auto actual =
        scraper.Parse("https://builds.limetech.io/?p=tf2items");

    CompareScraperData(expected, actual);
}
}  // namespace smam
