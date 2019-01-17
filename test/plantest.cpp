#include <string>
#include <vector>
#include <iostream>

#include "gtest/gtest.h"
#include "../src/plan.h"

Plan plan = Plan({
	"https://somelinkidunno.net",
	"c:config.cfg", "p:binary.smx", "e:extension.so"
});

TEST(PlanTest, LinkTest){
	EXPECT_EQ("https://somelinkidunno.net", plan.getUrl());
}

TEST(PlanTest, ConfigsTest){
	EXPECT_EQ("config.cfg", plan.getFileName(0));
	EXPECT_EQ("configs", plan.getFileDir(0));
	EXPECT_EQ('c', plan.getFileTag(0));
}

TEST(PlanTest, BinaryTest){
	EXPECT_EQ("binary.smx", plan.getFileName(1));
	EXPECT_EQ("plugins", plan.getFileDir(1));
	EXPECT_EQ('p', plan.getFileTag(1));
}

TEST(PlanTest, ExtensionTest){
	EXPECT_EQ("extension.so", plan.getFileName(2));
	EXPECT_EQ("extensions", plan.getFileDir(2));
	EXPECT_EQ('e', plan.getFileTag(2));
}

TEST(PlanTest, SetUrlTest){
	plan.setFileUrl(0, "https://link.net/config.cfg");
	plan.setFileUrl(1, "https://link.net/binary.smx");
	plan.setFileUrl(2, "https://link.net/extension.so");
	EXPECT_EQ("https://link.net/config.cfg", plan.getFileUrl(0));
	EXPECT_EQ("https://link.net/binary.smx", plan.getFileUrl(1));
	EXPECT_EQ("https://link.net/extension.so", plan.getFileUrl(2));
}
