project(SMAM_src_scrapers)

include(FindPkgConfig)

pkg_check_modules(LIBXML++ libxml++-3.0)
if(NOT DEFINED LIBXML++_LIBRARIES)
  pkg_check_modules(LIBXML++ REQUIRED libxml++-2.6)
endif()

pkg_check_modules(JSONCPP jsoncpp)

# Scraper base
add_library(scraper scraper.cpp)
target_link_libraries(scraper PRIVATE downloader)

# AlliedModders scraper
add_library(amscraper amscraper.cpp)
target_link_libraries(amscraper PRIVATE scraper utils_printer utils_version ${LIBXML++_LIBRARIES})
target_include_directories(amscraper PRIVATE ${LIBXML++_INCLUDE_DIRS})

# LimeTech scraper
add_library(ltscraper ltscraper.cpp)
target_link_libraries(ltscraper PRIVATE scraper utils_printer ${LIBXML++_LIBRARIES})
target_include_directories(ltscraper PRIVATE ${LIBXML++_INCLUDE_DIRS})

# GitHub scraper
add_library(ghscraper ghscraper.cpp)
target_link_libraries(ghscraper PRIVATE scraper utils_printer ${JSONCPP_LIBRARIES})
