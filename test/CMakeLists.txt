set(CURL_LIBRARY "-lcurl")
find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})

add_executable(tests)

target_sources(tests
  PRIVATE
    downloadermock.cpp
    utilstest.cpp
    amscrapertest.cpp
)

target_link_libraries(tests
  gtest_main
  ${CURL_LIBRARIES} # required for linkage, unused in mock
  utils
  downloader
  amscraper
)

add_test(
  NAME unit_tests
  COMMAND ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/tests
)
