add_executable(tests
  unit/data/file.cpp
  unit/data/addon.cpp
  unit/utils/options.cpp
  unit/utils/path.cpp
  # unit/scrapers/amscraper.cpp TODO: needs mocking
  # unit/scrapers/ltscraper.cpp
  # unit/scrapers/ghscraper.cpp
  #unit/operations/core.cpp
  unit/operations/common.cpp
  #unit/operations/addon.cpp TODO: needs mocking
  #unit/operations/installer.cpp
  # unit/operations/remover.cpp (fully integration tested)
  # network operations easier covered by pytest
)

target_include_directories(tests PRIVATE ${CONAN_INCLUDE_DIRS_JSONFORMODERNCPP})

target_link_libraries(tests
  CONAN_PKG::gtest

  # Utilities
  utils_logger
  utils_common
  utils_path
  utils_options

  # Scrapers
  #scraper_am TODO: needs mocking
  #scraper_lt
  #scraper_gh

  # Operations
  op_common
  #op_addon TODO: needs mocking
  #op_installer
  #op_remover

  # Data
  data_file
  data_addon
)
