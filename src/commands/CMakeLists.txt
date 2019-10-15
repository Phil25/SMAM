add_library(cmd_common common.cpp)
target_link_libraries(cmd_common PRIVATE utils_options cmd_install cmd_remove cmd_info cmd_search)

add_library(cmd_install install.cpp)
target_link_libraries(cmd_install op_installer)

add_library(cmd_remove remove.cpp)
target_link_libraries(cmd_remove op_remover)

add_library(cmd_info info.cpp)

add_library(cmd_search search.cpp)
