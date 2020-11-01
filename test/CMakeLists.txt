add_executable(tests)

target_sources(tests
  PRIVATE
    "unit/data/file.cpp"
    "unit/data/addon.cpp"
    "unit/utils/options.cpp"
    "unit/utils/path.cpp"
    "unit/operations/common.cpp"
)

target_link_libraries(tests PRIVATE smamlib CONAN_PKG::gtest)
