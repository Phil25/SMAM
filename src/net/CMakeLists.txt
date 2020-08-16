add_library(net_download download.cpp)

target_link_libraries(net_download PRIVATE CONAN_PKG::libcurl)

add_library(net_database database.cpp)
target_link_libraries(net_database PRIVATE net_download data_addon)
