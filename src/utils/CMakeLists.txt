project(SMAM_src_utils)

# Utils misc
add_library(utils_misc)
target_sources(utils_misc
  PRIVATE misc.cpp
  PUBLIC ${CMAKE_CURRENT_LIST_DIR}/misc.h
)
target_include_directories(utils_misc PUBLIC ${CMAKE_CURRENT_LIST_DIR})

# Utils archive
add_library(utils_version)
target_sources(utils_version
  PRIVATE version.cpp
  PUBLIC ${CMAKE_CURRENT_LIST_DIR}/version.h
)
target_include_directories(utils_version PUBLIC ${CMAKE_CURRENT_LIST_DIR})

# Utils archive
add_library(utils_archive)
target_sources(utils_archive
  PRIVATE archive.cpp
  PUBLIC ${CMAKE_CURRENT_LIST_DIR}/archive.h
)
target_include_directories(utils_archive PUBLIC ${CMAKE_CURRENT_LIST_DIR})
