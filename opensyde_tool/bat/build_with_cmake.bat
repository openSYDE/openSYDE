rem build cmake application
rem parameters
rem %1 Relative project folder path
rem %2 Relative temporary folder
rem %3 Build target (Release/Debug)
rem delete build folder and build using cmake
rem as a side effect this will also create the required configuration files for PC-lint
@echo off
SET CWD=%~dp0
if not exist %2 md %2

rem clean folder ...
del /s /f /q %2\*.* > nul

rem build using cmake
rem as a side effect this will also create the required configuration files for PC-lint
@echo off
SET CWD=%~dp0
if not exist %2 md %2

cd %2
rem set path to C,C++,resource compilers
SET PATH=C:\Qt\Qt6.8.3\Tools\mingw1310_64\bin\;%PATH%
rem set path to cmake
SET PATH=C:\Qt\Qt6.8.3\Tools\CMake_64\bin;%PATH%
rem set path to ninja
SET PATH=C:\Qt\Qt6.8.3\Tools\Ninja;%PATH%
rem set path to cmake Qt utilities
SET PATH=c:\Qt\Qt6.8.3\6.8.3\mingw_64\;%PATH%

rem run cmake
cmake.exe %1 -GNinja -DCMAKE_BUILD_TYPE=%3 -DCMAKE_TOOLCHAIN_FILE=%4
if ERRORLEVEL 1 goto done

rem perform actual build
cmake.exe --build . --target all -- -j4
if ERRORLEVEL 1 goto done

rem copy resulting binary to result:
cmake.exe --build . --target install
if ERRORLEVEL 1 goto done

cd ..\bat
:done

if errorlevel 1 exit /B
