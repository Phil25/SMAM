add_executable(tests
  unit/data/file.cpp
  unit/data/addon.cpp
  unit/operations/common.cpp
)

target_include_directories(tests PRIVATE ${CONAN_INCLUDE_DIRS_JSONFORMODERNCPP})

target_link_libraries(tests
  CONAN_PKG::gtest

  # General
  utils_logger
  data_file
  data_addon
)
