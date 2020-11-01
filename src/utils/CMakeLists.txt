target_sources(smamlib
  PRIVATE
    "archive.cpp"
    "archive.h"
    "codes.hpp"
    "common.cpp"
    "common.h"
    "error.hpp"
    "logger.cpp"
    "logger.h"
    "options.cpp"
    "options.h"
    "path.cpp"
    "path.h"
)

target_include_directories(smamlib PRIVATE ${CMAKE_CURRENT_LIST_DIR})
