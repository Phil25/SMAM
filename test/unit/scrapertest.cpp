#include "scrapertestcommon.hpp"

#ifdef NDEBUG
#undef NDEBUG  // make downloader use curlmock
#endif

#include "../../src/scrapers/amscraper.h"
#include "../../src/scrapers/ghscraper.h"
#include "../../src/scrapers/ltscraper.h"

using Data = Scraper::Data;

TEST(ScraperTest, AlliedModders)
{
    Scraper::add(std::make_unique<AMScraper>());

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

    Data actual = Scraper::getData(
        "https://forums.alliedmods.net/showpost.php?p=708265");

    compare(expected, actual);
}

TEST(ScraperTest, LimeTech)
{
    Scraper::add(std::make_unique<LTScraper>());

    Data expected;

    expected["connect-1.2.0-hg38-linux.zip"] =
        "https://builds.limetech.io/files/"
        "connect-1.2.0-hg38-linux.zip";

    expected.website = Data::Website::Limetech;

    Data actual =
        Scraper::getData("https://builds.limetech.io/?p=connect");

    compare(expected, actual);
}

TEST(ScraperTest, GitHub)
{
    Scraper::add(std::make_unique<GHScraper>());

    Data expected;

    expected["tf2attributes.smx"] =
        "https://github.com/FlaminSarge/tf2attributes/releases/"
        "download/v1.2.1/tf2attributes.smx";

    expected["tf2attributes_example.smx"] =
        "https://github.com/FlaminSarge/tf2attributes/releases/"
        "download/v1.2.1/tf2attributes_example.smx";

    expected.website = Data::Website::GitHub;

    Data actual = Scraper::getData(
        "https://github.com/FlaminSarge/tf2attributes/");

    compare(expected, actual);
}

TEST(ScraperTest, Unknown)
{
    Data expected;
    expected.website = Data::Website::Unknown;

    Data actual = Scraper::getData(
        "https://bitbucket.org/GoD_Tony/updater/downloads/");

    compare(expected, actual);
}
