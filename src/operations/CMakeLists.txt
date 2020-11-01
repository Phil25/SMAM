target_sources(smamlib
  PRIVATE
    "addon.cpp"
    "addon.h"
    "common.cpp"
    "common.h"
    "core.hpp"
    "installer.cpp"
    "installer.h"
    "remover.cpp"
    "remover.h"
)

target_include_directories(smamlib PRIVATE ${CMAKE_CURRENT_LIST_DIR})
