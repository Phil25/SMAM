add_library(net_download download.cpp)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
  target_link_libraries(net_download PRIVATE mock_curl)
else()
  target_link_libraries(net_download PRIVATE CONAN_PKG::libcurl)
endif()

add_library(net_database database.cpp)
