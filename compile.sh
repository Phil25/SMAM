#!/bin/bash

if [[ ! -d debug ]]; then
	mkdir debug && cd debug
	conan install .. --build missing -s build_type=Debug -s compiler=clang -s compiler.libcxx=libstdc++11 -s compiler.version=8
fi

cd debug
conan build ..
cd -
