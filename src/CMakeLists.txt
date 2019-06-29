project(SMAM_src)

include(FindPkgConfig)
pkg_check_modules(JSONCPP jsoncpp)

find_package(Boost 1.58 COMPONENTS program_options REQUIRED)
find_package(CURL REQUIRED)

# Options
add_library(opts opts.cpp)
target_link_libraries(opts PRIVATE boost_program_options)

# Download
add_library(download download.cpp)

if(CMAKE_BUILD_TYPE STREQUAL Release)
  target_link_libraries(download PRIVATE curl)
else()
  target_link_libraries(download PRIVATE curlmock)
endif()

# Database
add_library(database database.cpp)
target_link_libraries(database PRIVATE download ${JSONCPP_LIBRARIES})

# Commands
add_subdirectory(commands)

# Utilities
add_subdirectory(utils)

# Scrapers
add_subdirectory(scrapers)

# Main
add_executable(smam main.cpp)
target_link_libraries(smam PRIVATE cmd_common cmd_install cmd_remove cmd_info cmd_search)
