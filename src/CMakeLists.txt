add_subdirectory(net)
add_subdirectory(data)
add_subdirectory(utils)
add_subdirectory(commands)
add_subdirectory(operations)

add_executable(smam main.cpp)
target_link_libraries(smam PRIVATE utils_options utils_logger cmd_common)
