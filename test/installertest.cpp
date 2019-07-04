#include "gmock/gmock.h"
#include "gtest/gtest.h"

#ifdef NDEBUG
#undef NDEBUG  // make downloader use curlmock
#endif

#include "../src/commands/helpers/installer.h"

using namespace testing;

inline bool operator==(const File& f1, const File& f2) noexcept
{
    return f1.path == f2.path && f1.name == f2.name && f1.url == f2.url;
}

inline auto getFilesOfOne(const std::string& id) noexcept
    -> std::vector<File>
{
    auto i = Installer("localhost:7666", {id}, false);
    return i.getAddonFiles(id);
}

TEST(InstallerTest, Accelerator)
{
    File f = {"../../;accelerator-2.4.3-git127-b302f00-linux.zip",
              "https://builds.limetech.io/files/"
              "accelerator-2.4.3-git127-b302f00-linux.zip"};

    ASSERT_THAT(getFilesOfOne("accelerator"), ElementsAre(f));
}

TEST(InstallerTest, AdvancedInfiniteAmmo)
{
    File f = {"plugins/;AdvancedInfiniteAmmo.smx",
              "http://www.sourcemod.net/vbcompiler.php?file_id=148649"};

    ASSERT_THAT(getFilesOfOne("advancedinfiniteammo"), ElementsAre(f));
}

TEST(InstallerTest, AFKManager)
{
    File f1 = {
        "plugins/;afk_manager4.smx",
        "http://afkmanager.dawgclan.net/plugins/afk_manager4.smx"};

    File f2 = {"translations/;afk_manager.phrases.txt",
               "http://afkmanager.dawgclan.net/translations/"
               "afk_manager.phrases.txt"};

    ASSERT_THAT(getFilesOfOne("afk_manager"), ElementsAre(f1, f2));
}

TEST(InstallerTest, Connect)
{
    File f = {"../../;connect-1.2.0-hg38-linux.zip",
              "https://builds.limetech.io/files/"
              "connect-1.2.0-hg38-linux.zip"};

    ASSERT_THAT(getFilesOfOne("connect"), ElementsAre(f));
}

TEST(InstallerTest, DynamicMotd)
{
    File f = {"plugins/;dynamic_motd.smx",
              "https://forums.alliedmods.net/"
              "attachment.php?s=a59969c161c4e0ac23ef19c7cbb9ee5e&"
              "attachmentid=160284&d=1485099001"};

    ASSERT_THAT(getFilesOfOne("dynamic_motd"), ElementsAre(f));
}

TEST(InstallerTest, FuncommandsX)
{
    File f = {"./;funcommandsX_2.5.zip",
              "https://forums.alliedmods.net/"
              "attachment.php?s=dc5547938c38333b03dff89b0ac17723&"
              "attachmentid=159900&d=1483676185"};

    ASSERT_THAT(getFilesOfOne("funcommandsx"), ElementsAre(f));
}

TEST(InstallerTest, SteamTools)
{
    File f = {"../../;steamtools-0.10.0-git179-54fdc51-linux.zip",
              "https://builds.limetech.io/files/"
              "steamtools-0.10.0-git179-54fdc51-linux.zip"};

    ASSERT_THAT(getFilesOfOne("steamtools"), ElementsAre(f));
}

TEST(InstallerTest, TF2Attributes)
{
    File f1 = {"plugins/;tf2attributes.smx",
               "https://github.com/FlaminSarge/tf2attributes/releases/"
               "download/v1.2.1/tf2attributes.smx"};

    File f2 = {"gamedata/;tf2.attributes.txt",
               "https://raw.githubusercontent.com/FlaminSarge/"
               "tf2attributes/master/tf2.attributes.txt"};

    ASSERT_THAT(getFilesOfOne("tf2attributes"), ElementsAre(f1, f2));
}

TEST(InstallerTest, TF2Items)
{
    File f = {"../../;tf2items-1.6.4-hg279-linux.zip",
              "https://builds.limetech.io/files/"
              "tf2items-1.6.4-hg279-linux.zip"};

    ASSERT_THAT(getFilesOfOne("tf2items"), ElementsAre(f));
}

TEST(InstallerTest, Thriller)
{
    File f1 = {
        "plugins/;thriller.smx",
        "http://www.sourcemod.net/vbcompiler.php?file_id=128466"};

    File f2 = {"gamedata/;thriller.plugin.txt",
               "https://forums.alliedmods.net/"
               "attachment.php?s=a59969c161c4e0ac23ef19c7cbb9ee5e&"
               "attachmentid=133555&d=1400274898"};

    ASSERT_THAT(getFilesOfOne("thriller"), ElementsAre(f1, f2));
}

TEST(InstallerTest, Updater)
{
    File f = {"plugins/;updater.smx",
              "https://bitbucket.org/GoD_Tony/updater/downloads/"
              "updater.smx"};

    ASSERT_THAT(getFilesOfOne("updater"), ElementsAre(f));
}
