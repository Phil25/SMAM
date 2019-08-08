#include "scrapertest.hpp"

#ifdef NDEBUG
#undef NDEBUG  // make downloader use curlmock
#endif

#include "../../src/scrapers/ltscraper.h"

static LTScraper scraper;
using Data = Scraper::Data;

TEST(LTScraperTest, Accelerator)
{
    Data expected;
    expected["accelerator-2.4.3-git127-b302f00-linux.zip"] =
        "https://builds.limetech.io/files/"
        "accelerator-2.4.3-git127-b302f00-linux.zip";

    expected.website = Data::Website::Limetech;

    Data actual =
        scraper.fetch("https://builds.limetech.io/?p=accelerator");

    compare(expected, actual);
}

TEST(LTScraperTest, Connect)
{
    Data expected;

    expected["connect-1.2.0-hg38-linux.zip"] =
        "https://builds.limetech.io/files/"
        "connect-1.2.0-hg38-linux.zip";

    expected.website = Data::Website::Limetech;

    Data actual =
        scraper.fetch("https://builds.limetech.io/?p=connect");

    compare(expected, actual);
}

TEST(LTScraperTest, SteamTools)
{
    Data expected;

    expected["steamtools-0.10.0-git179-54fdc51-linux.zip"] =
        "https://builds.limetech.io/files/"
        "steamtools-0.10.0-git179-54fdc51-linux.zip";

    expected.website = Data::Website::Limetech;

    Data actual =
        scraper.fetch("https://builds.limetech.io/?p=steamtools");

    compare(expected, actual);
}

TEST(LTScraperTest, TF2Items)
{
    Data expected;

    expected["tf2items-1.6.4-hg279-linux.zip"] =
        "https://builds.limetech.io/files/"
        "tf2items-1.6.4-hg279-linux.zip";

    expected.website = Data::Website::Limetech;

    Data actual =
        scraper.fetch("https://builds.limetech.io/?p=tf2items");

    compare(expected, actual);
}
