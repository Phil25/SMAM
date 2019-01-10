add_executable(tests
  plantest.cpp
)

target_link_libraries(tests
  gtest_main
  plan
)

add_test(
  NAME unit_tests
  COMMAND ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/tests
)
