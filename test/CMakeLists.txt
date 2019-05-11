add_executable(tests)

target_sources(tests
  PRIVATE

# Mocks
  curlmock.cpp

# Tests
  optstest.cpp
  utilstest.cpp
  downloadertest.cpp
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
  utils_printer
  utils_smfs
  utils_version

# Thrid party
  -lcurl
  -lboost_program_options
  -lstdc++fs
)

add_test(
  NAME unit_tests
  COMMAND ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/tests
)
