#!/usr/bin/env bash

clear

export CC=/usr/bin/clang
export CXX=/usr/bin/clang++

rm -rf build
mkdir build
pushd build

conan install .. --build=missing -c tools.system.package_manager:mode=install -c tools.system.package_manager:sudo=True -pr clang-profile
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=True
cmake --build .
popd
ln -s build/compile_commands.json compile_commands.json
build/bin/PROJECT_NAME