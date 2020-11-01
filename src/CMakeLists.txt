find_package(Boost COMPONENTS program_options REQUIRED)

add_library(smamlib STATIC)

add_subdirectory(net)
add_subdirectory(data)
add_subdirectory(utils)
add_subdirectory(commands)
add_subdirectory(operations)
add_subdirectory(scrapers)

target_link_libraries(smamlib
  PRIVATE
    Boost::program_options
    CONAN_PKG::libcurl
    CONAN_PKG::OpenSSL
    CONAN_PKG::libzip
    CONAN_PKG::pugixml
)

add_executable(smam main.cpp)
target_link_libraries(smam PRIVATE smamlib)
