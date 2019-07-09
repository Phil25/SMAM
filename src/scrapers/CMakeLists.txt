project(SMAM_src_scrapers)

# Scraper base
add_library(scraper scraper.cpp)
target_link_libraries(scraper PRIVATE download)

# AlliedModders scraper
add_library(amscraper amscraper.cpp)
target_link_libraries(amscraper PRIVATE scraper utils_version CONAN_PKG::pugixml)

# LimeTech scraper
add_library(ltscraper ltscraper.cpp)
target_link_libraries(ltscraper PRIVATE scraper CONAN_PKG::pugixml)

# GitHub scraper
add_library(ghscraper ghscraper.cpp)
target_link_libraries(ghscraper PRIVATE scraper utils_printer)
target_include_directories(ghscraper PRIVATE ${CONAN_INCLUDE_DIRS_JSONFORMODERNCPP})
