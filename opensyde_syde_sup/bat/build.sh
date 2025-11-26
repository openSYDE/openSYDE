#!/bin/bash
#script usage:
# * ./build.sh -> build a cmake configuration, compile project and install build target
# * ./build.sh -l -> "-l(int_config)" -> only build cmake configuration (needed for pc_lint_plus scripts)
# * ./build.sh -t rpi -> cross-compile for Raspberry Pi (32-bit)
# * ./build.sh -t rpi64 -> cross-compile for Raspberry Pi (64-bit)

build_only_lint_config=false
target_platform="ubuntu"

# exit with error code if a command fails
set -e

while getopts ':lt:' flag; do
 case "$flag" in
    l)
        echo "Only build cmake configuration for pc_lint usage"
        build_only_lint_config=true
        ;;
    t)
        target_platform="${OPTARG}"
        echo "Target platform set to: ${target_platform}"
        ;;
   \?)
        echo "script usage:"
        echo "./build.sh -> compile project for Ubuntu (default)"
        echo "./build.sh -l -> -l(int_config) -> only build cmake configuration (for pc_lint usage)"
        echo "./build.sh -t ubuntu -> compile for Ubuntu (default)"
        echo "./build.sh -t rpi -> cross-compile for Raspberry Pi (32-bit ARM)"
        echo "./build.sh -t rpi64 -> cross-compile for Raspberry Pi (64-bit ARM)"
        exit 1
        ;;
    esac
done

echo "create build directory 'temp'"
rm -rf ../temp
mkdir ../temp
cd ../temp

# this step is always needed
# Select toolchain file based on target platform
case "${target_platform}" in
    ubuntu)
        echo "run cmake with toolchain file for 64bit Ubuntu compilation"
        TOOLCHAIN_FILE="../pjt/toolchain_ubuntu.cmake"
        ;;
    rpi|raspberrypi)
        echo "run cmake with toolchain file for Raspberry Pi (32-bit) cross-compilation"
        TOOLCHAIN_FILE="../pjt/toolchain_raspberrypi.cmake"
        ;;
    rpi64|raspberrypi64)
        echo "run cmake with toolchain file for Raspberry Pi (64-bit) cross-compilation"
        TOOLCHAIN_FILE="../pjt/toolchain_raspberrypi64.cmake"
        ;;
    *)
        echo "Unknown target platform: ${target_platform}"
        echo "Supported platforms: ubuntu, rpi, rpi64"
        exit 1
        ;;
esac

cmake ../pjt -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN_FILE} 

# only build the whole thing if no flag was given
if [ "$build_only_lint_config" = false ] ; then
    echo "build and install"
    cmake --build . --target all -- -j4
    cmake --build . --target install
fi

#return
cd ../bat
