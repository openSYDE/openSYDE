#!/bin/bash
#script usage: 
# * ./build.sh -> build a cmake configuration, compile project and install build target
# * ./build.sh -l -> "-l(int_config)" -> only build cmake configuration (needed for pc_lint_plus scripts)

build_only_lint_config=false

while getopts ':l' flag; do
 case "$flag" in
    l)
        echo "Only build cmake configuration for pc_lint usage"
        build_only_lint_config=true
        ;;
   \?)
        echo "script usage:"
        echo "./build.sh -> compile project"
        echo "./build.sh -l -> -l(int_config) -> only build cmake configuration (for pc_lint usage)"
        exit 1
        ;;
    esac
done

echo "create build directory 'temp'"
rm -rf ../temp
mkdir ../temp
cd ../temp

# this step is always needed
echo "run cmake with toolchain file for 64bit compilation"

cmake ../pjt -DCMAKE_TOOLCHAIN_FILE=../pjt/toolchain_ubuntu.cmake 

# only build the whole thing if no flag was given
if [ "$build_only_lint_config" = false ] ; then
    echo "build and install"
    cmake --build . --target all -- -j4
    cmake --build . --target install
fi

#return
cd ../bat
