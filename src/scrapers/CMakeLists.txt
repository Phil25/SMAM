project(SMAM_src_scrapers)

add_library(amscraper STATIC amscraper.cpp amscraper.h scraper.hpp)
target_sources(amscraper
  PRIVATE amscraper.cpp
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/amscraper.h
    ${CMAKE_CURRENT_LIST_DIR}/scraper.hpp
)
target_include_directories(amscraper PUBLIC ${CMAKE_CURRENT_LIST_DIR})
