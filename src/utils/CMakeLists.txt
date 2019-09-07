add_library(utils_options options.cpp)
target_link_libraries(utils_options PRIVATE CONAN_PKG::boost)

add_library(utils_logger logger.cpp)
