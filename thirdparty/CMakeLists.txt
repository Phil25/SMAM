set(CXXOPTS_URL "https://raw.githubusercontent.com/jarro2783/cxxopts/v2.1.2/include/cxxopts.hpp")

if(NOT EXISTS ${CMAKE_CURRENT_LIST_DIR}/cxxopts.hpp)
  message("Downloading cxxopts.hpp from ${CXXOPTS_URL}")
  file(DOWNLOAD ${CXXOPTS_URL} ${CMAKE_CURRENT_LIST_DIR}/cxxopts.hpp)
endif()
