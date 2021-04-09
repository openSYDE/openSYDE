#!/bin/bash

# create build directory "temp"
rm -rf ../temp
mkdir ../temp
cd ../temp

# run cmake with toolchain file to force 32 bit compilation
cmake ../pjt -DCMAKE_TOOLCHAIN_FILE=../pjt/toolchain_ubuntu.cmake 

# build and install
cmake --build . --target all -- -j4
cmake --build . --target install

# return
cd ../bat
