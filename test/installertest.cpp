#include "gtest/gtest.h"

#include "downloadermock.h"

#include "../src/installer.h"

inline void compare(const File& expected, const File& actual)
{
	EXPECT_EQ(expected.name, actual.name);
	EXPECT_EQ(expected.tag, actual.tag);
	EXPECT_EQ(expected.url, actual.url);
}

using FileVector = const std::vector<File>;
inline void compare(FileVector& expected, FileVector& actual)
{
	int i = expected.size();
	ASSERT_EQ(i, actual.size());

	while(--i >= 0)
	{
		compare(expected[i], actual[i]);
	}
}

class InstallerTest : public ::testing::Test
{
protected:
	DownloaderMock downloader;
	Database db;

	InstallerTest():
		db(downloader)
	{
	}

	void SetUp() override
	{
		Installer::initScrapers(downloader);
	}
};

TEST_F(InstallerTest, Accelerator)
{
	std::vector<File> expected = {
		File("a:accelerator-2.4.3-git127-b302f00-linux.zip", "https://builds.limetech.io/files/accelerator-2.4.3-git127-b302f00-linux.zip"),
	};

	db.precache({"accelerator"});
	compare(expected, Installer::getFiles("accelerator", db));
}

TEST_F(InstallerTest, AdvancedInfiniteAmmo)
{
	std::vector<File> expected = {
		File("p:AdvancedInfiniteAmmo.smx", "http://www.sourcemod.net/vbcompiler.php?file_id=148649"),
	};

	db.precache({"advancedinfiniteammo"});
	compare(expected, Installer::getFiles("advancedinfiniteammo", db));
}

TEST_F(InstallerTest, AFKManager)
{
	std::vector<File> expected = {
		File("p:afk_manager4.smx", "http://afkmanager.dawgclan.net/plugins/afk_manager4.smx"),
		File("t:afk_manager.phrases.txt", "http://afkmanager.dawgclan.net/translations/afk_manager.phrases.txt"),
	};

	db.precache({"afk_manager"});
	compare(expected, Installer::getFiles("afk_manager", db));
}

TEST_F(InstallerTest, Connect)
{
	std::vector<File> expected = {
		File("a:connect-1.2.0-hg38-linux.zip", "https://builds.limetech.io/files/connect-1.2.0-hg38-linux.zip"),
	};

	db.precache({"connect"});
	compare(expected, Installer::getFiles("connect", db));
}

TEST_F(InstallerTest, DynamicMotd)
{
	std::vector<File> expected = {
		File("p:dynamic_motd.smx", "https://forums.alliedmods.net/attachment.php?s=a59969c161c4e0ac23ef19c7cbb9ee5e&attachmentid=160284&d=1485099001"),
	};

	db.precache({"dynamic_motd"});
	compare(expected, Installer::getFiles("dynamic_motd", db));
}

TEST_F(InstallerTest, FuncommandsX)
{
	std::vector<File> expected = {
		File("a:funcommandsX_2.5.zip", "https://forums.alliedmods.net/attachment.php?s=dc5547938c38333b03dff89b0ac17723&attachmentid=159900&d=1483676185"),
	};

	db.precache({"funcommandsx"});
	compare(expected, Installer::getFiles("funcommandsx", db));
}

TEST_F(InstallerTest, SteamTools)
{
	std::vector<File> expected = {
		File("a:steamtools-0.10.0-git179-54fdc51-linux.zip", "https://builds.limetech.io/files/steamtools-0.10.0-git179-54fdc51-linux.zip"),
	};

	db.precache({"steamtools"});
	compare(expected, Installer::getFiles("steamtools", db));
}

TEST_F(InstallerTest, TF2Attributes)
{
	std::vector<File> expected = {
		File("p:tf2attributes.smx", "https://github.com/FlaminSarge/tf2attributes/releases/download/v1.2.1/tf2attributes.smx"),
		File("g:tf2.attributes.txt", "https://raw.githubusercontent.com/FlaminSarge/tf2attributes/master/tf2.attributes.txt"),
	};

	db.precache({"tf2attributes"});
	compare(expected, Installer::getFiles("tf2attributes", db));
}

TEST_F(InstallerTest, TF2Items)
{
	std::vector<File> expected = {
		File("a:tf2items-1.6.4-hg279-linux.zip", "https://builds.limetech.io/files/tf2items-1.6.4-hg279-linux.zip"),
	};

	db.precache({"tf2items"});
	compare(expected, Installer::getFiles("tf2items", db));
}

TEST_F(InstallerTest, Thriller)
{
	std::vector<File> expected = {
		File("p:thriller.smx", "http://www.sourcemod.net/vbcompiler.php?file_id=128466"),
		File("g:thriller.plugin.txt", "https://forums.alliedmods.net/attachment.php?s=a59969c161c4e0ac23ef19c7cbb9ee5e&attachmentid=133555&d=1400274898"),
	};

	db.precache({"thriller"});
	compare(expected, Installer::getFiles("thriller", db));
}

TEST_F(InstallerTest, Updater)
{
	std::vector<File> expected = {
		File("p:updater.smx", "https://bitbucket.org/GoD_Tony/updater/downloads/updater.smx"),
	};

	db.precache({"updater"});
	compare(expected, Installer::getFiles("updater", db));
}
