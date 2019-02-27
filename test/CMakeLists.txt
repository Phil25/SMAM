add_executable(tests)

target_sources(tests
  PRIVATE

# Mocks
  downloadermock.cpp

# Tests
  utilstest.cpp
  databasetest.cpp
  amscrapertest.cpp
  ltscrapertest.cpp
  ghscrapertest.cpp
  installertest.cpp
)

target_link_libraries(tests
  gtest_main

# General
  downloader
  database
  installer

# Scrapers
  amscraper
  ltscraper
  ghscraper

# Utilities
  utils_misc
  utils_version

# Thrid party
  -lcurl
)

add_test(
  NAME unit_tests
  COMMAND ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/tests
)
