#include "scrapertest.hpp"

#ifdef NDEBUG
#undef NDEBUG  // make downloader use curlmock
#endif

#include "../../src/scrapers/ghscraper.h"

static GHScraper scraper;
using Data = Scraper::Data;

TEST(GHScraperTest, TF2Attributes)
{
    Data expected;

    expected["tf2attributes.smx"] =
        "https://github.com/FlaminSarge/tf2attributes/releases/"
        "download/v1.2.1/tf2attributes.smx";

    expected["tf2attributes_example.smx"] =
        "https://github.com/FlaminSarge/tf2attributes/releases/"
        "download/v1.2.1/tf2attributes_example.smx";

    expected.website = Data::Website::GitHub;

    Data actual =
        scraper.fetch("https://github.com/FlaminSarge/tf2attributes/");

    compare(expected, actual);
}
