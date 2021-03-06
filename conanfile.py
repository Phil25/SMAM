from conans import ConanFile, CMake, tools

class SourceModAddonManager(ConanFile):
    generators = "cmake"
    settings = "os", "compiler", "build_type", "arch"
    requires = (
        "cmake_installer/3.14.5@conan/stable",
        ("zlib/1.2.9@conan/stable", "override"),
        "boost/1.71.0@conan/stable",
        "gtest/1.8.1@bincrafters/stable",
        "pugixml/1.9@bincrafters/stable",
        "libzip/1.2.0@bincrafters/stable",
        "libcurl/7.64.1@bincrafters/stable",
        "jsonformoderncpp/3.6.1@vthiery/stable"
    )

    def build(self):
        cmake = CMake(self)

        if tools.get_env("ISOLATED", False):
            cmake.definitions["ISOLATED"] = 1

        cmake.definitions["CMAKE_BUILD_TYPE"] = str(self.settings.build_type)
        cmake.configure()
        cmake.build()
