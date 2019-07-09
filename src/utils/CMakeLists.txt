project(SMAM_src_utils)

# Archive
add_library(utils_archive archive.cpp)
target_link_libraries(utils_archive PRIVATE CONAN_PKG::libzip)

# Misc
add_library(utils_misc misc.cpp)

# Printer
add_library(utils_printer printer.cpp)

# SourceMod Filesystem
add_library(utils_smfs smfs.cpp)

# Version
add_library(utils_version version.cpp)
target_link_libraries(utils_version PRIVATE utils_misc)
