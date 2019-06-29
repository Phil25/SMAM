#include "gtest/gtest.h"

#ifdef NDEBUG
#undef NDEBUG  // make downloader use curlmock
#endif

#include "../src/commands/helpers/installer.h"

inline void compare(const File& expected, const File& actual) noexcept
{
    EXPECT_EQ(expected.path, actual.path);
    EXPECT_EQ(expected.name, actual.name);
    EXPECT_EQ(expected.url, actual.url);
}

using FileVector = const std::vector<File>;
inline void compare(FileVector& expected, FileVector& actual) noexcept
{
    int i = expected.size();
    ASSERT_EQ(i, actual.size());

    while (--i >= 0)
    {
        compare(expected[i], actual[i]);
    }
}

inline Installer getInstaller(
    const std::vector<std::string>& addons) noexcept
{
    return Installer("localhost:7666", addons, false);
}

TEST(InstallerTest, Accelerator)
{
    std::vector<File> expected = {
        File("../../;accelerator-2.4.3-git127-b302f00-linux.zip",
             "https://builds.limetech.io/files/"
             "accelerator-2.4.3-git127-b302f00-linux.zip"),
    };

    Installer installer = getInstaller({"accelerator"});
    // compare(expected, installer.getAddonFiles("accelerator"));
}

TEST(InstallerTest, AdvancedInfiniteAmmo)
{
    std::vector<File> expected = {
        File("plugins/;AdvancedInfiniteAmmo.smx",
             "http://www.sourcemod.net/vbcompiler.php?file_id=148649"),
    };

    Installer installer = getInstaller({"advancedinfiniteammo"});
    compare(expected, installer.getAddonFiles("advancedinfiniteammo"));
}

TEST(InstallerTest, AFKManager)
{
    std::vector<File> expected = {
        File("plugins/;afk_manager4.smx",
             "http://afkmanager.dawgclan.net/plugins/afk_manager4.smx"),
        File("translations/;afk_manager.phrases.txt",
             "http://afkmanager.dawgclan.net/translations/"
             "afk_manager.phrases.txt"),
    };

    Installer installer = getInstaller({"afk_manager"});
    compare(expected, installer.getAddonFiles("afk_manager"));
}

TEST(InstallerTest, Connect)
{
    std::vector<File> expected = {
        File("../../;connect-1.2.0-hg38-linux.zip",
             "https://builds.limetech.io/files/"
             "connect-1.2.0-hg38-linux.zip"),
    };

    Installer installer = getInstaller({"connect"});
    compare(expected, installer.getAddonFiles("connect"));
}

TEST(InstallerTest, DynamicMotd)
{
    std::vector<File> expected = {
        File("plugins/;dynamic_motd.smx",
             "https://forums.alliedmods.net/"
             "attachment.php?s=a59969c161c4e0ac23ef19c7cbb9ee5e&"
             "attachmentid=160284&d=1485099001"),
    };

    Installer installer = getInstaller({"dynamic_motd"});
    compare(expected, installer.getAddonFiles("dynamic_motd"));
}

TEST(InstallerTest, FuncommandsX)
{
    std::vector<File> expected = {
        File("./;funcommandsX_2.5.zip",
             "https://forums.alliedmods.net/"
             "attachment.php?s=dc5547938c38333b03dff89b0ac17723&"
             "attachmentid=159900&d=1483676185"),
    };

    Installer installer = getInstaller({"funcommandsx"});
    compare(expected, installer.getAddonFiles("funcommandsx"));
}

TEST(InstallerTest, SteamTools)
{
    std::vector<File> expected = {
        File("../../;steamtools-0.10.0-git179-54fdc51-linux.zip",
             "https://builds.limetech.io/files/"
             "steamtools-0.10.0-git179-54fdc51-linux.zip"),
    };

    Installer installer = getInstaller({"steamtools"});
    compare(expected, installer.getAddonFiles("steamtools"));
}

TEST(InstallerTest, TF2Attributes)
{
    std::vector<File> expected = {
        File("plugins/;tf2attributes.smx",
             "https://github.com/FlaminSarge/tf2attributes/releases/"
             "download/v1.2.1/tf2attributes.smx"),
        File("gamedata/;tf2.attributes.txt",
             "https://raw.githubusercontent.com/FlaminSarge/"
             "tf2attributes/master/tf2.attributes.txt"),
    };

    Installer installer = getInstaller({"tf2attributes"});
    compare(expected, installer.getAddonFiles("tf2attributes"));
}

TEST(InstallerTest, TF2Items)
{
    std::vector<File> expected = {
        File("../../;tf2items-1.6.4-hg279-linux.zip",
             "https://builds.limetech.io/files/"
             "tf2items-1.6.4-hg279-linux.zip"),
    };

    Installer installer = getInstaller({"tf2items"});
    compare(expected, installer.getAddonFiles("tf2items"));
}

TEST(InstallerTest, Thriller)
{
    std::vector<File> expected = {
        File("plugins/;thriller.smx",
             "http://www.sourcemod.net/vbcompiler.php?file_id=128466"),
        File("gamedata/;thriller.plugin.txt",
             "https://forums.alliedmods.net/"
             "attachment.php?s=a59969c161c4e0ac23ef19c7cbb9ee5e&"
             "attachmentid=133555&d=1400274898"),
    };

    Installer installer = getInstaller({"thriller"});
    compare(expected, installer.getAddonFiles("thriller"));
}

TEST(InstallerTest, Updater)
{
    std::vector<File> expected = {
        File("plugins/;updater.smx",
             "https://bitbucket.org/GoD_Tony/updater/downloads/"
             "updater.smx"),
    };

    Installer installer = getInstaller({"updater"});
    compare(expected, installer.getAddonFiles("updater"));
}
