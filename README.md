[![Build Status](https://travis-ci.org/Phil25/SMAM.svg?branch=master)](https://travis-ci.org/Phil25/SMAM)

# SourceMod Addon Manager

SourceMod Addon Manager (SMAM) is an automatic installer for [SourceMod](https://www.sourcemod.net/about.php) plugins or extensions.

# Build dependencies
* `g++-8` and `gcc-8` (C++17 Filesystem library)
* [libcurl](https://curl.haxx.se/libcurl/)
* [libxml++](http://libxmlplusplus.sourceforge.net/) (preferably version 3.0+)
* [jsoncpp](https://github.com/open-source-parsers/jsoncpp)
* [Boost.Program\_options](https://www.boost.org/doc/libs/release/doc/html/program_options.html)
* [libzip](https://libzip.org/)
* Ubuntu package names available in `.travis.yml` file.

# Building
1. Clone the repo and cd to it: `git clone https://github.com/Phil25/SMAM && cd SMAM`
1. Designate a directory for building: `mkdir build && cd build`
1. Setup CMake providing `Debug` or `Release` as `CMAKE_BUILD_TYPE`:
	* `cmake -DCMAKE_BUILD_TYPE=Debug ..` (required for automated tests)
	* `cmake -DCMAKE_BUILD_TYPE=Release ..`
1. Build the project: `cmake --build .`

# Running and testing
From the previously created `build` directory:
* Execute `./bin/smam` to run SMAM.
* Execute `./bin/tests` to run tests (only in `Debug` build).
* Call `../test/cmdinstalltest . ./bin/smam` to run execution tests.

# Usage
```
Usage:
  smam <command> [addons] [options]

Commands:
  install                              Install specified addons.
  remove                               Remove specified addons.
  search                               Search for addons.
  info                                 Get info about installed addons.

Options:
  -h [ --help ]                        Show help.
  --version                            Show version.
  -q [ --quiet ]                       Do not produce output.
  --no-prefix                          Disable prefixes in output.
  --no-color                           Disable color in output.
  --log arg                            Path to log file.
  -d [ --destination ] arg             Path to server.
  --db-url arg (=https://smamdb.net/)  URL of the database.
```
