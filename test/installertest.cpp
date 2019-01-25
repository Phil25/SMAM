#include "gtest/gtest.h"

#include "../src/installer.h"

#include "downloadermock.h"
#include "databasemock.h"


class InstallerTest : public ::testing::Test{
protected:
	DatabaseMock dbmock;
	DownloaderMock downmock;

	void SetUp() override{
		Installer::init(dbmock, downmock);
	}
};

TEST_F(InstallerTest, Init){
}
