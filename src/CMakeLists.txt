project(SMAM_src)

# Downloader
add_library(downloader)
target_sources(downloader
  PRIVATE downloader.cpp
  PUBLIC ${CMAKE_CURRENT_LIST_DIR}/downloader.h
)
target_include_directories(downloader PUBLIC ${CMAKE_CURRENT_LIST_DIR})

# Scrapers
add_subdirectory(scrapers)

# Plan
add_library(plan)
target_sources(plan
  PRIVATE plan.cpp
  PUBLIC ${CMAKE_CURRENT_LIST_DIR}/plan.h
)
target_include_directories(plan PUBLIC ${CMAKE_CURRENT_LIST_DIR})

# Installer
add_library(installer)
target_sources(installer
  PRIVATE installer.cpp
  PUBLIC ${CMAKE_CURRENT_LIST_DIR}/installer.h
)
target_include_directories(installer PUBLIC ${CMAKE_CURRENT_LIST_DIR})

# Utils
add_library(utils)
target_sources(utils
  PRIVATE utils.cpp
  PUBLIC ${CMAKE_CURRENT_LIST_DIR}/utils.h
)
target_include_directories(utils PUBLIC ${CMAKE_CURRENT_LIST_DIR})

# Main
add_executable(main main.cpp)
target_link_libraries(main PRIVATE utils plan downloader installer amscraper)
