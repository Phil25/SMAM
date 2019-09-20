add_library(scrapper_main scraper.cpp)

add_library(scrapper_am amscraper.cpp)
target_link_libraries(scrapper_am PRIVATE scrapper_main CONAN_PKG::pugixml)

add_library(scrapper_lt ltscraper.cpp)

add_library(scrapper_gh ghscraper.cpp)
