add_library(op_installer installer.cpp)
target_link_libraries(op_installer PRIVATE scraper_am scraper_lt scraper_gh net_database op_common op_addon)

add_library(op_remover remover.cpp)
target_link_libraries(op_remover PRIVATE op_common op_addon)

add_library(op_addon addon.cpp)
target_link_libraries(op_addon PRIVATE data_addon utils_path utils_archive)

add_library(op_common common.cpp)
