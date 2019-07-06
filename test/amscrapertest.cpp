#include "scrapertest.hpp"

#ifdef NDEBUG
#undef NDEBUG  // make downloader use curlmock
#endif

#include "../src/scrapers/amscraper.h"

static AMScraper scraper;
using Data = Scraper::Data;

TEST(AMScraperTest, AdvancedInfiniteAmmo)
{
    Data expected;

    expected["advancedinfiniteammo.inc"] =
        "https://forums.alliedmods.net/"
        "attachment.php?s=a59969c161c4e0ac23ef19c7cbb9ee5e&"
        "attachmentid=147135&d=1438556651";

    expected["AdvancedInfiniteAmmo.smx"] =
        "http://www.sourcemod.net/vbcompiler.php?file_id=148649";

    expected["AdvancedInfiniteAmmo.sp"] =
        "https://forums.alliedmods.net/"
        "attachment.php?s=a59969c161c4e0ac23ef19c7cbb9ee5e&"
        "attachmentid=148649&d=1444226746";

    expected.website = Data::Website::AlliedModders;

    Data actual = scraper.fetch(
        "https://forums.alliedmods.net/showpost.php?p=1754217");

    compare(expected, actual);
}

TEST(AMScraperTest, AFKManager)
{
    Data expected;

    expected["afk_manager_old.zip"] =
        "https://forums.alliedmods.net/"
        "attachment.php?s=dc5547938c38333b03dff89b0ac17723&"
        "attachmentid=160240&d=1484963606";

    expected["afk_manager.phrases.txt"] =
        "https://forums.alliedmods.net/"
        "attachment.php?s=dc5547938c38333b03dff89b0ac17723&"
        "attachmentid=166646&d=1510967008";

    expected["afk_manager.inc"] =
        "https://forums.alliedmods.net/"
        "attachment.php?s=dc5547938c38333b03dff89b0ac17723&"
        "attachmentid=168028&d=1516358726";

    expected["afk_manager4.smx"] =
        "http://www.sourcemod.net/vbcompiler.php?file_id=170330";

    expected["afk_manager4.sp"] =
        "https://forums.alliedmods.net/"
        "attachment.php?s=dc5547938c38333b03dff89b0ac17723&"
        "attachmentid=170330&d=1530622367";

    expected["afk_manager.changelog.txt"] =
        "https://forums.alliedmods.net/"
        "attachment.php?s=dc5547938c38333b03dff89b0ac17723&"
        "attachmentid=170331&d=1530622381";

    expected.website = Data::Website::AlliedModders;

    Data actual = scraper.fetch(
        "https://forums.alliedmods.net/showpost.php?p=708265");

    compare(expected, actual);
}

TEST(AMScraperTest, DynamicMotd)
{
    Data expected;

    expected["dynamic_motd.smx"] =
        "https://forums.alliedmods.net/"
        "attachment.php?s=a59969c161c4e0ac23ef19c7cbb9ee5e&"
        "attachmentid=160284&d=1485099001";

    expected["dynamic_motd.sp"] =
        "https://forums.alliedmods.net/"
        "attachment.php?s=a59969c161c4e0ac23ef19c7cbb9ee5e&"
        "attachmentid=160285&d=1485100105";

    expected.website = Data::Website::AlliedModders;

    Data actual = scraper.fetch(
        "https://forums.alliedmods.net/showpost.php?p=1387386");

    compare(expected, actual);
}

TEST(AMScraperTest, FuncommandsX)
{
    Data expected;

    expected["funcommandsX_2.5.zip"] =
        "https://forums.alliedmods.net/"
        "attachment.php?s=dc5547938c38333b03dff89b0ac17723&"
        "attachmentid=159900&d=1483676185";

    expected.website = Data::Website::AlliedModders;

    Data actual = scraper.fetch(
        "https://forums.alliedmods.net/showpost.php?p=665771");

    compare(expected, actual);
}

TEST(AMScraperTest, Thriller)
{
    Data expected;

    expected["thriller.plugin.txt"] =
        "https://forums.alliedmods.net/"
        "attachment.php?s=a59969c161c4e0ac23ef19c7cbb9ee5e&"
        "attachmentid=133555&d=1400274898";

    expected["thriller.smx"] =
        "http://www.sourcemod.net/vbcompiler.php?file_id=128466";

    expected["thriller.sp"] =
        "https://forums.alliedmods.net/"
        "attachment.php?s=a59969c161c4e0ac23ef19c7cbb9ee5e&"
        "attachmentid=128466&d=1386796191";

    expected.website = Data::Website::AlliedModders;

    Data actual = scraper.fetch(
        "https://forums.alliedmods.net/showpost.php?p=1590169");

    compare(expected, actual);
}
