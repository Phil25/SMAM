project(SMAM_src)

include(FindPkgConfig)
pkg_check_modules(JSONCPP jsoncpp)

find_package(Boost 1.58 COMPONENTS program_options REQUIRED)
find_package(CURL REQUIRED)

# Options
add_library(opts opts.cpp)
target_link_libraries(opts PRIVATE boost_program_options)

# Downloader
add_library(downloader downloader.cpp)
target_include_directories(downloader PRIVATE curl)

# Database
add_library(database database.cpp)
target_link_libraries(database PRIVATE downloader ${JSONCPP_LIBRARIES})

# Installer
add_library(installer installer.cpp)
target_link_libraries(installer PRIVATE database scraper amscraper ltscraper ghscraper utils_printer utils_version)

# Commands
add_subdirectory(commands)

# Utilities
add_subdirectory(utils)

# Scrapers
add_subdirectory(scrapers)

# Main
add_executable(smam main.cpp)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
  target_sources(smam PRIVATE ../test/curlmock.cpp)
endif()

target_link_libraries(smam PRIVATE curl cmd_common cmd_install cmd_remove cmd_info cmd_search)
