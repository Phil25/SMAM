target_sources(smamlib
  PRIVATE
    "amscraper.cpp"
    "amscraper.h"
    "ghscraper.cpp"
    "ghscraper.h"
    "ltscraper.cpp"
    "ltscraper.h"
    "scraper.cpp"
    "scraper.h"
)

target_include_directories(smamlib PRIVATE ${CMAKE_CURRENT_LIST_DIR})
