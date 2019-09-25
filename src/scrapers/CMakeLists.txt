add_library(scraper_main scraper.cpp)

add_library(scraper_am amscraper.cpp)
target_link_libraries(scraper_am PRIVATE scraper_main CONAN_PKG::pugixml utils_common)

add_library(scraper_lt ltscraper.cpp)

add_library(scraper_gh ghscraper.cpp)
