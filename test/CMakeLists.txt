add_executable(tests
  unit/filetest.cpp
  unit/addontest.cpp
)

target_include_directories(tests PRIVATE ${CONAN_INCLUDE_DIRS_JSONFORMODERNCPP})

target_link_libraries(tests
  CONAN_PKG::gtest

  # General
  data_file
  data_addon
)
