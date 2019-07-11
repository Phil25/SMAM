project(SMAM_src_cmd_helpers)

add_library(report report.cpp)

add_library(installer installer.cpp)
target_link_libraries(installer PRIVATE report database smfs amscraper ltscraper ghscraper utils_printer utils_archive utils_misc utils_version)
