add_library(mock_curl mock/curl.cpp)
target_include_directories(mock_curl PRIVATE ${CONAN_INCLUDE_DIRS_JSONFORMODERNCPP})

add_executable(tests
  unit/mock/curl.cpp
  unit/net/download.cpp
  unit/net/database.cpp
  unit/data/file.cpp
  unit/data/addon.cpp
  unit/utils/options.cpp
  unit/utils/path.cpp
  unit/operations/core.cpp
  unit/operations/common.cpp
)

target_include_directories(tests PRIVATE ${CONAN_INCLUDE_DIRS_JSONFORMODERNCPP})

target_link_libraries(tests
  CONAN_PKG::gtest

  # Utilities
  utils_logger
  utils_common
  utils_path
  utils_options

  # Operations
  op_common

  # Data
  data_file
  data_addon

  # Network
  net_download
  net_database

  # Other
  mock_curl
)
