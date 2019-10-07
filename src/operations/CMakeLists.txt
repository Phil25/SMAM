add_library(op_installer installer.cpp)
target_link_libraries(op_installer PRIVATE scraper_am scraper_lt scraper_gh net_database op_common op_addon)

add_library(op_addon addon.cpp)
target_link_libraries(op_addon PRIVATE utils_path data_addon)

add_library(op_common common.cpp)
