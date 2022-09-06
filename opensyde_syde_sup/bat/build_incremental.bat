rem build using cmake
rem as a side effect this will also create the required configuration files for PC-lint
@echo off
SET CWD=%~dp0
if not exist ..\temp md ..\temp

cd ..\temp
rem set path to C,C++,resource compilers
SET PATH=C:\Qt\Qt5.15.2\Tools\mingw810_32\bin\;%PATH%
rem set path to cmake
SET PATH=C:\Qt\Qt5.15.2\Tools\CMake_64\bin;%PATH%
rem set path to ninja
SET PATH=C:\Qt\Qt5.15.2\Tools\ninja;%PATH%

rem run cmake
cmake.exe ..\pjt -GNinja -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../pjt/toolchain_windows.cmake
if ERRORLEVEL 1 goto done

rem perform actual build
cmake.exe --build . --target all -- -j4
if ERRORLEVEL 1 goto done

rem copy resulting binary to result:
cmake.exe --build . --target install
if ERRORLEVEL 1 goto done

cd ..\bat
:done