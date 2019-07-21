project(SMAM_src_addon)

# Addon
add_library(addon addon.cpp)
target_link_libraries(addon PRIVATE path file)

# Path
add_library(path path.cpp)
target_link_libraries(path PRIVATE stdc++fs)

# File
add_library(file file.cpp)
