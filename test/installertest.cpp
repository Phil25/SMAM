#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../src/commands/helpers/installer.h"

using namespace testing;

inline auto get(const std::string& id, const std::string& url) noexcept
{
    return Installer("localhost:7666", {id}, false).get(url);
}

TEST(InstallerTest, Accelerator)
{
    EXPECT_THAT(
        get("accelerator", "https://builds.limetech.io/?p=accelerator"),
        ElementsAre(
            Pair("accelerator-2.4.3-git127-b302f00-linux.zip",
                 "https://builds.limetech.io/files/"
                 "accelerator-2.4.3-git127-b302f00-linux.zip")));
}

TEST(InstallerTest, AdvancedInfiniteAmmo)
{
    EXPECT_THAT(
        get("advancedinfiniteammo",
            "https://forums.alliedmods.net/showpost.php?p=1754217"),
        ElementsAre(
            Pair("AdvancedInfiniteAmmo.smx",
                 "http://www.sourcemod.net/"
                 "vbcompiler.php?file_id=148649"),
            Pair("AdvancedInfiniteAmmo.sp",
                 "https://forums.alliedmods.net/"
                 "attachment.php?s=a59969c161c4e0ac23ef19c7cbb9ee5e&"
                 "attachmentid=148649&d=1444226746"),
            Pair("advancedinfiniteammo.inc",
                 "https://forums.alliedmods.net/"
                 "attachment.php?s=a59969c161c4e0ac23ef19c7cbb9ee5e&"
                 "attachmentid=147135&d=1438556651")));
}

TEST(InstallerTest, AFKManager)
{
    EXPECT_THAT(
        get("afk_manager",
            "https://forums.alliedmods.net/showpost.php?p=708265"),
        ElementsAre(
            Pair("afk_manager.changelog.txt",
                 "https://forums.alliedmods.net/"
                 "attachment.php?s=dc5547938c38333b03dff89b0ac17723&"
                 "attachmentid=170331&d=1530622381"),
            Pair("afk_manager.inc",
                 "https://forums.alliedmods.net/"
                 "attachment.php?s=dc5547938c38333b03dff89b0ac17723&"
                 "attachmentid=168028&d=1516358726"),
            Pair("afk_manager.phrases.txt",
                 "https://forums.alliedmods.net/"
                 "attachment.php?s=dc5547938c38333b03dff89b0ac17723&"
                 "attachmentid=166646&d=1510967008"),
            Pair("afk_manager4.smx",
                 "http://www.sourcemod.net/"
                 "vbcompiler.php?file_id=170330"),
            Pair("afk_manager4.sp",
                 "https://forums.alliedmods.net/"
                 "attachment.php?s=dc5547938c38333b03dff89b0ac17723&"
                 "attachmentid=170330&d=1530622367"),
            Pair("afk_manager_old.zip",
                 "https://forums.alliedmods.net/"
                 "attachment.php?s=dc5547938c38333b03dff89b0ac17723&"
                 "attachmentid=160240&d=1484963606")));
}

TEST(InstallerTest, Connect)
{
    EXPECT_THAT(get("connect", "https://builds.limetech.io/?p=connect"),
                ElementsAre(Pair("connect-1.2.0-hg38-linux.zip",
                                 "https://builds.limetech.io/files/"
                                 "connect-1.2.0-hg38-linux.zip")));
}

TEST(InstallerTest, DynamicMotd)
{
    EXPECT_THAT(
        get("dynamic_motd",
            "https://forums.alliedmods.net/showpost.php?p=1387386"),
        ElementsAre(
            Pair("dynamic_motd.smx",
                 "https://forums.alliedmods.net/"
                 "attachment.php?s=a59969c161c4e0ac23ef19c7cbb9ee5e&"
                 "attachmentid=160284&d=1485099001"),
            Pair("dynamic_motd.sp",
                 "https://forums.alliedmods.net/"
                 "attachment.php?s=a59969c161c4e0ac23ef19c7cbb9ee5e&"
                 "attachmentid=160285&d=1485100105")));
}

TEST(InstallerTest, FuncommandsX)
{
    EXPECT_THAT(
        get("funcommandsx",
            "https://forums.alliedmods.net/showpost.php?p=665771"),
        ElementsAre(
            Pair("funcommandsX_2.5.zip",
                 "https://forums.alliedmods.net/"
                 "attachment.php?s=dc5547938c38333b03dff89b0ac17723&"
                 "attachmentid=159900&d=1483676185")));
}

TEST(InstallerTest, SteamTools)
{
    EXPECT_THAT(
        get("steamtools", "https://builds.limetech.io/?p=steamtools"),
        ElementsAre(
            Pair("steamtools-0.10.0-git179-54fdc51-linux.zip",
                 "https://builds.limetech.io/files/"
                 "steamtools-0.10.0-git179-54fdc51-linux.zip")));
}

TEST(InstallerTest, TF2Attributes)
{
    EXPECT_THAT(
        get("tf2attributes",
            "https://github.com/FlaminSarge/tf2attributes/"),
        ElementsAre(
            Pair("tf2attributes.smx",
                 "https://github.com/FlaminSarge/tf2attributes/"
                 "releases/download/v1.2.1/tf2attributes.smx"),
            Pair(
                "tf2attributes_example.smx",
                "https://github.com/FlaminSarge/tf2attributes/releases/"
                "download/v1.2.1/tf2attributes_example.smx")));
}

TEST(InstallerTest, TF2Items)
{
    EXPECT_THAT(
        get("tf2items", "https://builds.limetech.io/?p=tf2items"),
        ElementsAre(Pair("tf2items-1.6.4-hg279-linux.zip",
                         "https://builds.limetech.io/files/"
                         "tf2items-1.6.4-hg279-linux.zip")));
}

TEST(InstallerTest, Thriller)
{
    EXPECT_THAT(
        get("thriller",
            "https://forums.alliedmods.net/showpost.php?p=1590169"),
        ElementsAre(
            Pair("thriller.plugin.txt",
                 "https://forums.alliedmods.net/"
                 "attachment.php?s=a59969c161c4e0ac23ef19c7cbb9ee5e&"
                 "attachmentid=133555&d=1400274898"),
            Pair("thriller.smx",
                 "http://www.sourcemod.net/"
                 "vbcompiler.php?file_id=128466"),
            Pair("thriller.sp",
                 "https://forums.alliedmods.net/"
                 "attachment.php?s=a59969c161c4e0ac23ef19c7cbb9ee5e&"
                 "attachmentid=128466&d=1386796191")));
}

TEST(InstallerTest, Updater)
{
    EXPECT_THAT(
        get("updater",
            "https://bitbucket.org/GoD_Tony/updater/downloads/"),
        IsEmpty());  // data not set during fallback
}
