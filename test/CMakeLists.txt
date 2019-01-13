set(CURL_LIBRARY "-lcurl")
find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})

add_executable(tests)

target_sources(tests
  PRIVATE
    utilstest.cpp
    scrapertest.cpp
    amscrapertest.cpp
    plantest.cpp
)

target_link_libraries(tests
  gtest_main
  ${CURL_LIBRARIES} # required for linkage, unused in mock
  utils
  downloader
  amscraper
  plan
)

add_test(
  NAME unit_tests
  COMMAND ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/tests
)
