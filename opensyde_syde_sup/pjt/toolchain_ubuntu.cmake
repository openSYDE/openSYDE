# cmake toolchain file for ubuntu 64 bit compilation
# usage: cmake ../pjt -DCMAKE_TOOLCHAIN_FILE=../pjt/toolchain_ubuntu.cmake 

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR amd64)

# use gcc-9 and g++-9 explicitly; that was what SYDEsup was originally tested with
# change to "gcc" and "g++" to use default compilers
set(CMAKE_C_COMPILER gcc-9)
set(CMAKE_C_FLAGS "-m64 -Wno-deprecated-declarations -fPIC")
set(CMAKE_CXX_COMPILER g++-9)
set(CMAKE_CXX_FLAGS "-m64 -Wno-deprecated-declarations -fPIC")

set(CMAKE_SHARED_LINKER_FLAGS "-m64")

