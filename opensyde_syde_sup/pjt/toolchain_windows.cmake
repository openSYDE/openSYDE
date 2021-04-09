# cmake toolchain file for ubuntu 32 bit compilation
# usage: cmake ../pjt -DCMAKE_TOOLCHAIN_FILE=../pjt/toolchain_ubuntu.cmake 

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR i386)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_C_FLAGS -m32)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_FLAGS -m32)

set(CMAKE_SHARED_LINKER_FLAGS "-m32")

# link standard libraries statically
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")

