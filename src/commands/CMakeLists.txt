project(SMAM_src_cmd)

add_library(cmd_common common.cpp)
target_link_libraries(cmd_common PRIVATE opts)

add_library(cmd_install install.cpp)
target_link_libraries(cmd_install PRIVATE utils_report database amscraper ltscraper ghscraper)

add_library(cmd_remove remove.cpp)

add_library(cmd_info info.cpp)

add_library(cmd_search search.cpp)
