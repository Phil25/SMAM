project(SMAM_src_scrapers)

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
target_include_directories(amscraper PUBLIC ${CMAKE_CURRENT_LIST_DIR})
