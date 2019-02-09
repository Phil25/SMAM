project(SMAM_src_utils)

include(FindPkgConfig)
pkg_check_modules(LIBXML++ libxml++-3.0)
if(NOT DEFINED LIBXML++_LIBRARIES)
  pkg_check_modules(LIBXML++ REQUIRED libxml++-2.6)
endif()

# Utils
add_library(utilsmisc)
target_sources(utilsmisc
  PRIVATE misc.cpp
  PUBLIC ${CMAKE_CURRENT_LIST_DIR}/misc.h
)
target_include_directories(utilsmisc PUBLIC ${CMAKE_CURRENT_LIST_DIR})

add_library(utilsversion)
target_sources(utilsversion
  PRIVATE version.cpp
  PUBLIC ${CMAKE_CURRENT_LIST_DIR}/version.h
)
target_include_directories(utilsversion PUBLIC ${CMAKE_CURRENT_LIST_DIR})

add_library(utilsamnode)
target_sources(utilsamnode
  PRIVATE amnode.cpp
  PUBLIC ${CMAKE_CURRENT_LIST_DIR}/amnode.h
)
target_link_libraries(utilsamnode ${LIBXML++_LIBRARIES})
target_include_directories(utilsamnode PUBLIC ${CMAKE_CURRENT_LIST_DIR} ${LIBXML++_INCLUDE_DIRS})
