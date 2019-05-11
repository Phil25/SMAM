#include <string>
#include <vector>
#include <iostream>

#include "gtest/gtest.h"
#include "../src/utils/file.hpp"
#include "../src/utils/misc.h"
#include "../src/utils/smfs.h"
#include "../src/utils/version.h"

TEST(UtilsTest, FileConstr)
{
	File f("some/dir/;file;name.txt");
	EXPECT_EQ("some/dir/", f.path);
	EXPECT_EQ("file;name.txt", f.name);
}

TEST(UtilsTest, ToLines)
{
	std::vector<std::string> vec = Utils::toLines("Line1\nLine2\nLine3");
	ASSERT_EQ(3, vec.size());

	EXPECT_EQ("Line1", vec.at(0));
	EXPECT_EQ("Line2", vec.at(1));
	EXPECT_EQ("Line3", vec.at(2));
}

TEST(UtilsTest, IsLink)
{
	EXPECT_TRUE(Utils::isLink("https://sth"));
	EXPECT_TRUE(Utils::isLink("http://sth"));

	EXPECT_FALSE(Utils::isLink("https:/sth"));
	EXPECT_FALSE(Utils::isLink("http:/sth"));

	EXPECT_FALSE(Utils::isLink("ttps://sth"));
	EXPECT_FALSE(Utils::isLink("ttp://sth"));

	EXPECT_FALSE(Utils::isLink(" https://sth"));
	EXPECT_FALSE(Utils::isLink(" http://sth"));

	EXPECT_FALSE(Utils::isLink("ahttps://sth"));
	EXPECT_FALSE(Utils::isLink("ahttp://sth"));
}

TEST(UtilsTest, Extract)
{
	std::string value = Utils::extract("abcvaluedef", "abc", "def");
	EXPECT_EQ("value", value);

	std::string comment = Utils::extract("/*comment*/", "/*", "*/");
	EXPECT_EQ("comment", comment);

	std::string ln2 = Utils::extract("ln1\nln2\nln3", "ln1", "ln3");
	EXPECT_EQ("\nln2\n", ln2);

	std::string noextract1 = Utils::extract("No extract", "", "");
	EXPECT_EQ("No extract", noextract1);

	std::string noextract2 = Utils::extract("No extract", "No", "");
	EXPECT_EQ("No extract", noextract2);

	std::string noextract3 = Utils::extract("No extract", "", "extract");
	EXPECT_EQ("No extract", noextract3);
}

TEST(UtilsTest, FindRoot)
{
	namespace fs = SMFS::fs;
	ASSERT_TRUE(fs::create_directories("./mod/addons/sourcemod/plugins"));

	auto equal = [](const fs::path& p)
	{
		static const fs::path root = "./mod/addons/sourcemod";

		auto mp = SMFS::findRoot(p);
		return mp.has_value() && fs::equivalent(mp.value(), root);
	};

	EXPECT_FALSE(SMFS::findRoot({}));
	EXPECT_FALSE(equal(""));
	EXPECT_FALSE(equal("."));
	EXPECT_FALSE(equal("./"));

	EXPECT_TRUE(equal("./mod"));
	EXPECT_TRUE(equal("./mod/addons"));
	EXPECT_TRUE(equal("./mod/addons/sourcemod"));
	EXPECT_TRUE(equal("./mod/addons/sourcemod/plugins"));

	fs::remove_all("./mod/");
}

TEST(UtilsTest, IsPathSafe)
{
	EXPECT_TRUE(SMFS::isPathSafe(""));
	EXPECT_TRUE(SMFS::isPathSafe("."));
	EXPECT_TRUE(SMFS::isPathSafe("./"));
	EXPECT_TRUE(SMFS::isPathSafe(".."));
	EXPECT_TRUE(SMFS::isPathSafe("../"));
	EXPECT_TRUE(SMFS::isPathSafe("../."));
	EXPECT_TRUE(SMFS::isPathSafe(".././"));
	EXPECT_TRUE(SMFS::isPathSafe("../.."));
	EXPECT_TRUE(SMFS::isPathSafe("../../"));
	EXPECT_TRUE(SMFS::isPathSafe("../../."));
	EXPECT_TRUE(SMFS::isPathSafe("../.././"));

	EXPECT_FALSE(SMFS::isPathSafe("../../.."));
	EXPECT_FALSE(SMFS::isPathSafe("../../../"));
	EXPECT_FALSE(SMFS::isPathSafe("../../../."));
	EXPECT_FALSE(SMFS::isPathSafe("../../.././"));
	EXPECT_FALSE(SMFS::isPathSafe("../../../.."));

	EXPECT_FALSE(SMFS::isPathSafe(".././../.."));
	EXPECT_FALSE(SMFS::isPathSafe("../.././../"));
	EXPECT_FALSE(SMFS::isPathSafe(".././.././../."));
	EXPECT_FALSE(SMFS::isPathSafe(".././../../././"));
	EXPECT_FALSE(SMFS::isPathSafe("../../.././././.."));
}

TEST(UtilsTest, VersionBiggest)
{
	std::vector<std::string> vers0 = {"3.1"};
	EXPECT_EQ("3.1", Utils::Version::biggest(vers0));

	std::vector<std::string> vers = {
		"2.5", "2.14", "2.1",
	};
	EXPECT_EQ("2.14", Utils::Version::biggest(vers));

	std::vector<std::string> vers2 = {
		"2.5.6-99", "2.5.6-104", "2.5.6-20", "2.1-200", "2.5.6-103"
	};
	EXPECT_EQ("2.5.6-104", Utils::Version::biggest(vers2));
}

TEST(UtilsTest, VersionBiggestName)
{
	std::vector<std::string> versions = {
		"name_2.51-0.zip",
		"name_2.51-1.zip",
		"name_1.52.zip",
		"name_1.99-1.zip",
	};
	EXPECT_EQ("name_2.51-1.zip", Utils::Version::biggest(versions));
}
