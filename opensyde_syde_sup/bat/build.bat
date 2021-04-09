rem build using cmake
rem as a side effect this will also create the required configuration files for PC-lint
@echo off
SET CWD=%~dp0
if not exist ..\temp md ..\temp

rem clean folder ...
del /s /f /q ..\temp\*.*

cd ..\temp
rem set path to C,C++,resource compilers
SET PATH=%PATH%;C:\Qt\Qt5.7.0\Tools\mingw530_32\bin\
rem set path to cmake
SET PATH=%PATH%;%CWD%\cmake\bin\
rem set path to ninja
SET PATH=%PATH%;%CWD%\

rem run cmake
cmake.exe ..\pjt -GNinja -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../pjt/toolchain_windows.cmake

rem perform actual build
cmake.exe --build . --target all -- -j4
rem copy resulting binary to result:
cmake.exe --build . --target install
cd ..\bat
