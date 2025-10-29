rem build GUI application
time /t

set PROJECT_FOLDER=..\pjt\openSYDE
set TEMP_FOLDER=..\temp_openSYDE_Release
set BUILD_TARGET=Release
set TOOLCHAIN=../pjt/toolchain_windows.cmake

call build_with_cmake %PROJECT_FOLDER% %TEMP_FOLDER% %BUILD_TARGET% %TOOLCHAIN%

time /t
