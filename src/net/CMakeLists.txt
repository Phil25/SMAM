target_sources(smamlib
  PRIVATE
    "download.cpp"
    "download.h"
    "database.cpp"
    "database.h"
)

target_include_directories(smamlib PRIVATE ${CMAKE_CURRENT_LIST_DIR})
