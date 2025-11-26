# CMake toolchain file for Raspberry Pi cross-compilation from macOS
# This toolchain file supports cross-compiling from macOS (darwin) to ARM-based Raspberry Pi
#
# Prerequisites:
# 1. Install ARM cross-compilation toolchain:
#    - For Homebrew: brew install arm-linux-gnueabihf-binutils
#    - Or install full toolchain from ARM/Linaro
#
# Usage:
#    cmake ../pjt -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../pjt/toolchain_raspberrypi.cmake

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Specify the cross-compiler
# For Raspberry Pi 3/4 (32-bit): use arm-linux-gnueabihf
# For Raspberry Pi 3/4 (64-bit): use aarch64-linux-gnu
set(CROSS_COMPILE_PREFIX "arm-linux-gnueabihf")

# Try to find the cross-compiler in common locations
find_program(CMAKE_C_COMPILER
    NAMES ${CROSS_COMPILE_PREFIX}-gcc
    PATHS
        /usr/local/bin
        /opt/homebrew/bin
        /usr/bin
        $ENV{HOME}/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin
        $ENV{ARM_TOOLCHAIN_PATH}/bin
)

find_program(CMAKE_CXX_COMPILER
    NAMES ${CROSS_COMPILE_PREFIX}-g++
    PATHS
        /usr/local/bin
        /opt/homebrew/bin
        /usr/bin
        $ENV{HOME}/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin
        $ENV{ARM_TOOLCHAIN_PATH}/bin
)

# Compiler flags for ARM (Raspberry Pi 3/4 with ARMv7/ARMv8 in 32-bit mode)
# -march=armv7-a: ARMv7 architecture (Pi 2/3/4)
# -mfpu=neon-vfpv4: NEON SIMD and VFPv4 floating point
# -mfloat-abi=hard: Use hardware floating point
set(CMAKE_C_FLAGS "-march=armv7-a -mfpu=neon-vfpv4 -mfloat-abi=hard -Wno-deprecated-declarations -fPIC" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS "-march=armv7-a -mfpu=neon-vfpv4 -mfloat-abi=hard -Wno-deprecated-declarations -fPIC" CACHE STRING "" FORCE)

# For Raspberry Pi Zero/1 (ARMv6), uncomment these instead:
# set(CMAKE_C_FLAGS "-march=armv6 -mfpu=vfp -mfloat-abi=hard -Wno-deprecated-declarations -fPIC" CACHE STRING "" FORCE)
# set(CMAKE_CXX_FLAGS "-march=armv6 -mfpu=vfp -mfloat-abi=hard -Wno-deprecated-declarations -fPIC" CACHE STRING "" FORCE)

# For Raspberry Pi 4 (64-bit), use aarch64 toolchain and these flags:
# set(CROSS_COMPILE_PREFIX "aarch64-linux-gnu")
# set(CMAKE_SYSTEM_PROCESSOR aarch64)
# set(CMAKE_C_FLAGS "-march=armv8-a -Wno-deprecated-declarations -fPIC" CACHE STRING "" FORCE)
# set(CMAKE_CXX_FLAGS "-march=armv8-a -Wno-deprecated-declarations -fPIC" CACHE STRING "" FORCE)

# Linker flags
set(CMAKE_SHARED_LINKER_FLAGS "" CACHE STRING "" FORCE)
set(CMAKE_EXE_LINKER_FLAGS "" CACHE STRING "" FORCE)

# Search for programs only in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# Search for libraries and headers only in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Set the install directory for Raspberry Pi builds
set(INSTALL_SUBDIR "raspberrypi")

# Display information about the toolchain
message(STATUS "=================================================")
message(STATUS "Raspberry Pi Cross-Compilation Toolchain")
message(STATUS "=================================================")
message(STATUS "System: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Processor: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C Flags: ${CMAKE_C_FLAGS}")
message(STATUS "CXX Flags: ${CMAKE_CXX_FLAGS}")
message(STATUS "Install Directory: result/${INSTALL_SUBDIR}")
message(STATUS "=================================================")

# Check if compilers were found
if(NOT CMAKE_C_COMPILER)
    message(FATAL_ERROR "ARM cross-compiler not found! Please install the toolchain.\n"
        "For macOS with Homebrew:\n"
        "  brew install arm-linux-gnueabihf-binutils\n"
        "Or download from: https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain\n"
        "You can also set ARM_TOOLCHAIN_PATH environment variable to point to your toolchain.")
endif()

if(NOT CMAKE_CXX_COMPILER)
    message(FATAL_ERROR "ARM cross-compiler not found! Please install the toolchain.\n"
        "For macOS with Homebrew:\n"
        "  brew install arm-linux-gnueabihf-binutils\n"
        "Or download from: https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain\n"
        "You can also set ARM_TOOLCHAIN_PATH environment variable to point to your toolchain.")
endif()
