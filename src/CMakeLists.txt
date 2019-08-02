project(SMAM_src)

# Options
add_library(opts opts.cpp)
target_link_libraries(opts PRIVATE CONAN_PKG::boost)

# Download
add_library(download download.cpp)

if(CMAKE_BUILD_TYPE STREQUAL Release)
  target_link_libraries(download PRIVATE CONAN_PKG::libcurl)
else()
  target_link_libraries(download PRIVATE curlmock)
endif()

# Database
add_library(database database.cpp)
target_link_libraries(database PRIVATE download addon)

# Commands
add_subdirectory(commands)

# SourceMod Filesystem
add_subdirectory(smfs)

# Scrapers
add_subdirectory(scrapers)

# Utilities
add_subdirectory(utils)

# Main
add_executable(smam main.cpp)
target_link_libraries(smam PRIVATE cmd_common cmd_install cmd_remove cmd_info cmd_search)
