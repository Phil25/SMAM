#include <string>
#include <vector>
#include <iostream>

#include "gtest/gtest.h"
#include "../src/utils.h"

TEST(UtilsTest, Extract){
	std::string value = Utils::extract("abcvaluedef", "abc", "def");
	EXPECT_EQ("value", value);

	std::string comment = Utils::extract("/*comment*/", "/*", "*/");
	EXPECT_EQ("comment", comment);

	std::string ln2 = Utils::extract("ln1\nln2\nln3", "ln1", "ln3");
	EXPECT_EQ("\nln2\n", ln2);
}

TEST(UtilsTest, BiggestVer){
	std::vector<std::string> vers0{"3.1"};
	EXPECT_EQ("3.1", Utils::biggestVer(vers0));

	std::vector<std::string> vers{
		"2.5", "2.14", "2.1",
	};
	EXPECT_EQ("2.14", Utils::biggestVer(vers));

	std::vector<std::string> vers2{
		"2.5.6-99", "2.5.6-104", "2.5.6-20", "2.1-200", "2.5.6-103"
	};
	EXPECT_EQ("2.5.6-104", Utils::biggestVer(vers2));
}
