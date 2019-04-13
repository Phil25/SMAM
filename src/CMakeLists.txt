project(SMAM_src)

include(FindPkgConfig)
pkg_check_modules(JSONCPP jsoncpp)

find_package(Boost 1.58 COMPONENTS program_options REQUIRED)

# Downloader
add_library(downloader)
target_sources(downloader
  PRIVATE downloader.cpp
  PUBLIC ${CMAKE_CURRENT_LIST_DIR}/downloader.h
)
target_include_directories(downloader PUBLIC ${CMAKE_CURRENT_LIST_DIR})

# Database
add_library(database)
target_sources(database
  PRIVATE database.cpp
  PUBLIC ${CMAKE_CURRENT_LIST_DIR}/database.h
)
target_link_libraries(database ${JSONCPP_LIBRARIES})
target_include_directories(database PUBLIC ${CMAKE_CURRENT_LIST_DIR} ${JSONCPP_INCLUDE_DIRS})

# Installer
add_library(installer)
target_sources(installer
  PRIVATE installer.cpp downloader.cpp database.cpp
  PUBLIC ${CMAKE_CURRENT_LIST_DIR}/installer.h
)
target_include_directories(installer PUBLIC downloader database ${CMAKE_CURRENT_LIST_DIR})

# Utilities
add_subdirectory(utils)

# Scrapers
add_subdirectory(scrapers)

# Main
add_executable(smam main.cpp)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
  target_sources(smam PRIVATE ../test/downloadermock.cpp)
endif()

target_link_libraries(smam
# General
  downloader
  database
  installer

# Scrapers
  amscraper
  ltscraper
  ghscraper

# Utilities
  utils_misc
  utils_version

# Thrid party
  -lcurl
  -lboost_program_options
)
