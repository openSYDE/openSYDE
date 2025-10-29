# cmake toolchain file for Windows 64 bit compilation
# usage: cmake ../pjt -DCMAKE_TOOLCHAIN_FILE=../pjt/toolchain_windows.cmake 

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR amd64)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_C_FLAGS "-m64 -Wno-deprecated-declarations")
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_FLAGS "-m64 -Wno-deprecated-declarations")

set(CMAKE_SHARED_LINKER_FLAGS "-m64")

# link standard libraries statically
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")

