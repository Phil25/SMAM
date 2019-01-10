project(SMAM_src)

# PROCESS SCRAPERS
add_subdirectory(scrapers)

# PLAN
add_library(plan)
target_sources(plan
  PRIVATE plan.cpp
  PUBLIC ${CMAKE_CURRENT_LIST_DIR}/plan.h
)
target_include_directories(plan PUBLIC ${CMAKE_CURRENT_LIST_DIR})

# DOWNLOADER
add_library(downloader)
target_sources(downloader
  PRIVATE downloader.cpp
  PUBLIC ${CMAKE_CURRENT_LIST_DIR}/downloader.h
)
target_include_directories(downloader PUBLIC ${CMAKE_CURRENT_LIST_DIR})

# INSTALLER
add_library(installer)
target_sources(installer
  PRIVATE installer.cpp
  PUBLIC ${CMAKE_CURRENT_LIST_DIR}/installer.h
)
target_include_directories(installer PUBLIC ${CMAKE_CURRENT_LIST_DIR})

# MAIN
add_executable(main main.cpp)
target_link_libraries(main PRIVATE plan downloader installer amscraper)
