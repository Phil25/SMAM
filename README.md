[![Build Status](https://travis-ci.org/Phil25/SMAM.svg?branch=master)](https://travis-ci.org/Phil25/SMAM)

# SourceMod Addon Manager

SourceMod Addon Manager (or SMAM) is an automatic installer for [SourceMod](https://www.sourcemod.net/about.php) plugins or extensions.

# Building
1. Download the repo and cd to it: `git pull github.com/Phil25/SMAM && cd SMAM`
1. Designate a directory for building: `mkdir build && cd build`
1. Setup CMake providing `Debug` or `Release` as `CMAKE_BUILD_TYPE`:
	* `cmake -DCMAKE_BUILD_TYPE=Debug ..` (required for automated tests)
	* `cmake -DCMAKE_BUILD_TYPE=Release ..`
1. Build the project: `cmake --build .`

# Running and testing
* Execute `./bin/smam` to run SMAM.
* Execute `./bin/tests` to run tests (only in `Debug` build).
* Call `../test/cmdinstalltest . ./bin/smam` to run execution tests.
