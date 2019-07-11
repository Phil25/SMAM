project(SMAM_src_cmd)

add_library(cmd_common common.cpp)
target_link_libraries(cmd_common PRIVATE opts stdc++fs)

add_library(cmd_install install.cpp)
target_link_libraries(cmd_install PRIVATE cmd_common installer)

add_library(cmd_remove remove.cpp)
target_link_libraries(cmd_remove PRIVATE cmd_common smfs)

add_library(cmd_info info.cpp)
target_link_libraries(cmd_info PRIVATE cmd_common smfs)

add_library(cmd_search search.cpp)
target_link_libraries(cmd_search PRIVATE cmd_common)

# Helpers
add_subdirectory(helpers)
