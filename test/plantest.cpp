#include <string>
#include <vector>
#include <iostream>

#include "gtest/gtest.h"
#include "../src/plan.h"

class PlanTest : public testing::Test{
protected:
	Plan* thriller;

	void SetUp() override{
		std::vector<std::string> strings{
			"https://forums.alliedmods.net/showpost.php?p=1590169",
			"p:thriller.sp",
			"g:thriller.plugin.txt"
		};

		std::vector<char*> cstrings{};
		for(auto& string : strings)
			cstrings.push_back(&string.front());

		thriller = new Plan(cstrings.data(), cstrings.size());
	}

	void TearDown() override{
		delete thriller;
	}
};

TEST_F(PlanTest, ThrillerTest){
	EXPECT_EQ(2, thriller->size());
}
