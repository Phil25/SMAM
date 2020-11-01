target_sources(smamlib
  PRIVATE
    "common.cpp"
    "common.h"
    "info.cpp"
    "install.cpp"
    "remove.cpp"
    "search.cpp"
)

target_include_directories(smamlib PRIVATE ${CMAKE_CURRENT_LIST_DIR})
