project(SMAM_src_scrapers)

include(FindPkgConfig)
pkg_check_modules(LIBXML++ libxml++-3.0)
if(NOT DEFINED LIBXML++_LIBRARIES)
  pkg_check_modules(LIBXML++ REQUIRED libxml++-2.6)
endif()

add_library(scraper)
target_sources(scraper
  PRIVATE scraper.cpp
  PUBLIC ${CMAKE_CURRENT_LIST_DIR}/scraper.h
)
target_include_directories(scraper PUBLIC ${CMAKE_CURRENT_LIST_DIR})

add_library(amscraper)
target_sources(amscraper
  PRIVATE amscraper.cpp
  PUBLIC ${CMAKE_CURRENT_LIST_DIR}/amscraper.h
)
target_link_libraries(amscraper scraper ${LIBXML++_LIBRARIES})
target_include_directories(amscraper PUBLIC ${CMAKE_CURRENT_LIST_DIR} ${LIBXML++_INCLUDE_DIRS})

add_library(ltscraper)
target_sources(ltscraper
  PRIVATE ltscraper.cpp
  PUBLIC ${CMAKE_CURRENT_LIST_DIR}/ltscraper.h
)
target_link_libraries(ltscraper scraper ${LIBXML++_LIBRARIES})
target_include_directories(ltscraper PUBLIC ${CMAKE_CURRENT_LIST_DIR} ${LIBXML++_INCLUDE_DIRS})
