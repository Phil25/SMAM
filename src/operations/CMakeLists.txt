add_library(op_installer installer.cpp)
target_link_libraries(op_installer PRIVATE net_database)

add_library(op_common common.cpp)
