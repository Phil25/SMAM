project(SMAM_src_utils)

# Utils
add_library(utils_misc)
target_sources(utils_misc
  PRIVATE misc.cpp
  PUBLIC ${CMAKE_CURRENT_LIST_DIR}/misc.h
)
target_include_directories(utils_misc PUBLIC ${CMAKE_CURRENT_LIST_DIR})

add_library(utils_version)
target_sources(utils_version
  PRIVATE version.cpp
  PUBLIC ${CMAKE_CURRENT_LIST_DIR}/version.h
)
target_include_directories(utils_version PUBLIC ${CMAKE_CURRENT_LIST_DIR})
