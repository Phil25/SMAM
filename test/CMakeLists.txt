add_executable(tests)

target_sources(tests
  PRIVATE
    downloadermock.cpp
    databasemock.cpp
    utilstest.cpp
    amscrapertest.cpp
    plantest.cpp
)

target_link_libraries(tests
  gtest_main
  utils
  downloader
  database
  plan
  scraper
  amscraper
  -lcurl
)

add_test(
  NAME unit_tests
  COMMAND ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/tests
)
