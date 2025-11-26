# CMake toolchain file for Raspberry Pi 64-bit cross-compilation from macOS
# This toolchain file supports cross-compiling from macOS to 64-bit ARM-based Raspberry Pi (Pi 3/4/5 with 64-bit OS)
#
# Prerequisites:
# 1. Install ARM 64-bit cross-compilation toolchain:
#    - For Homebrew: brew install aarch64-linux-gnu-binutils
#
# Usage:
#    cmake ../pjt -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../pjt/toolchain_raspberrypi64.cmake

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

# Specify the cross-compiler for 64-bit ARM
set(CROSS_COMPILE_PREFIX "aarch64-linux-gnu")

# Try to find the cross-compiler in common locations
find_program(CMAKE_C_COMPILER
    NAMES ${CROSS_COMPILE_PREFIX}-gcc
    PATHS
        /opt/homebrew/bin
        /usr/local/bin
        /usr/bin
        $ENV{HOME}/tools/aarch64-linux-gnu/bin
        $ENV{ARM_TOOLCHAIN_PATH}/bin
)

find_program(CMAKE_CXX_COMPILER
    NAMES ${CROSS_COMPILE_PREFIX}-g++
    PATHS
        /opt/homebrew/bin
        /usr/local/bin
        /usr/bin
        $ENV{HOME}/tools/aarch64-linux-gnu/bin
        $ENV{ARM_TOOLCHAIN_PATH}/bin
)

# Compiler flags for AArch64 (64-bit ARM)
# -march=armv8-a: ARMv8 architecture (Raspberry Pi 3/4/5 with 64-bit OS)
set(CMAKE_C_FLAGS "-march=armv8-a -Wno-deprecated-declarations -fPIC" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS "-march=armv8-a -Wno-deprecated-declarations -fPIC" CACHE STRING "" FORCE)

# Linker flags
set(CMAKE_SHARED_LINKER_FLAGS "" CACHE STRING "" FORCE)
set(CMAKE_EXE_LINKER_FLAGS "" CACHE STRING "" FORCE)

# Set up ARM sysroot for libraries like OpenSSL
set(ARM_SYSROOT "${CMAKE_CURRENT_LIST_DIR}/../arm_sysroot")

# Check if ARM OpenSSL is available
if(EXISTS "${ARM_SYSROOT}/usr/lib/libcrypto.a")
    message(STATUS "Found ARM OpenSSL in ${ARM_SYSROOT}")
    set(CMAKE_FIND_ROOT_PATH ${ARM_SYSROOT})
    set(OPENSSL_ROOT_DIR "${ARM_SYSROOT}/usr")
    set(OPENSSL_INCLUDE_DIR "${ARM_SYSROOT}/usr/include")
    set(OPENSSL_CRYPTO_LIBRARY "${ARM_SYSROOT}/usr/lib/libcrypto.a")
    set(OPENSSL_SSL_LIBRARY "${ARM_SYSROOT}/usr/lib/libssl.a")

    # Add include path for OpenSSL headers
    include_directories(${OPENSSL_INCLUDE_DIR})
else()
    message(WARNING "ARM OpenSSL not found in ${ARM_SYSROOT}")
    message(WARNING "Please run: bat/build_openssl_for_arm.sh aarch64")
    message(WARNING "Attempting to use system OpenSSL (may cause build failures)")
endif()

# Search for programs only in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# Search for libraries and headers in target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Set the install directory for Raspberry Pi 64-bit builds
set(INSTALL_SUBDIR "raspberrypi64")

# Display information about the toolchain
message(STATUS "=================================================")
message(STATUS "Raspberry Pi 64-bit Cross-Compilation Toolchain")
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
    message(FATAL_ERROR "AArch64 cross-compiler not found! Please install the toolchain.\n"
        "For macOS with Homebrew:\n"
        "  brew install aarch64-linux-gnu-binutils\n"
        "Or download from: https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain\n"
        "You can also set ARM_TOOLCHAIN_PATH environment variable to point to your toolchain.")
endif()

if(NOT CMAKE_CXX_COMPILER)
    message(FATAL_ERROR "AArch64 cross-compiler not found! Please install the toolchain.\n"
        "For macOS with Homebrew:\n"
        "  brew install aarch64-linux-gnu-binutils\n"
        "Or download from: https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain\n"
        "You can also set ARM_TOOLCHAIN_PATH environment variable to point to your toolchain.")
endif()
