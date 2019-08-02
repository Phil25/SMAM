project(SMAM_src_scrapers)

# Scraper base
add_library(scraper scraper.cpp)

# AlliedModders scraper
add_library(amscraper amscraper.cpp)

# LimeTech scraper
add_library(ltscraper ltscraper.cpp)
target_link_libraries(ltscraper PRIVATE CONAN_PKG::pugixml)

# GitHub scraper
add_library(ghscraper ghscraper.cpp)
