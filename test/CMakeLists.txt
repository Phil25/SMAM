add_library(curlmock curlmock.cpp)
target_include_directories(curlmock PRIVATE ${CONAN_INCLUDE_DIRS_JSONFORMODERNCPP})

add_executable(tests
  curlmocktest.cpp
  optstest.cpp
  filetest.cpp
  pathtest.cpp
  addontest.cpp
  utilstest.cpp
  downloadtest.cpp
  databasetest.cpp
  amscrapertest.cpp
  ltscrapertest.cpp
  ghscrapertest.cpp
)

target_include_directories(tests PRIVATE ${CONAN_INCLUDE_DIRS_JSONFORMODERNCPP})

target_link_libraries(tests
  CONAN_PKG::gtest

# General
  opts
  download
  database
  addon
  file
  path
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
  utils_version
)

add_test(
  NAME unit_tests
  COMMAND ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/tests
)
