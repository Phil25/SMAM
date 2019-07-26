project(SMAM_src_addon)

# Addon
add_library(addon addon.cpp)
target_link_libraries(addon PRIVATE utils_archive utils_printer path file) # TODO: check if file needed

# Path
add_library(path path.cpp)

# File
add_library(file file.cpp)
target_link_libraries(file PRIVATE utils_misc utils_version scraper) # TODO: check if scraper needed
