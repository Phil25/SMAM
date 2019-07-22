project(SMAM_src_cmd_helpers)

add_library(report report.cpp)
target_link_libraries(report PRIVATE utils_printer) # TODO see if required

add_library(installer installer.cpp)
target_link_libraries(installer PRIVATE report database addon path amscraper ltscraper ghscraper utils_printer utils_archive utils_misc utils_version)
