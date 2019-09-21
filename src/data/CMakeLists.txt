add_library(data_file file.cpp)

add_library(data_addon addon.cpp)
target_link_libraries(data_addon PRIVATE data_file)
