target_sources(smamlib
  PRIVATE
    "addon.cpp"
    "addon.h"
    "file.cpp"
    "file.h"
)

target_include_directories(smamlib PRIVATE ${CMAKE_CURRENT_LIST_DIR})
