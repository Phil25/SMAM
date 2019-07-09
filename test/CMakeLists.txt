add_library(curlmock curlmock.cpp)

add_executable(tests
  curlmocktest.cpp
  optstest.cpp
  utilstest.cpp
  downloadtest.cpp
  databasetest.cpp
  amscrapertest.cpp
  ltscrapertest.cpp
  ghscrapertest.cpp
  installertest.cpp
)

target_link_libraries(tests
  CONAN_PKG::gtest

# General
  opts
  download
  database
  installer
  curlmock

# Commands
  cmd_install
  cmd_remove
  cmd_info
  cmd_search

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
  CONAN_PKG::boost
  stdc++fs
)

add_test(
  NAME unit_tests
  COMMAND ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/tests
)
