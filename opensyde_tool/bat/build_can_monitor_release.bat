rem build CAN Monitor application

set PROJECT_FOLDER=..\pjt\openSYDE_CAN_Monitor
set TEMP_FOLDER=..\temp_openSYDE_CAN_Monitor_Release
set BUILD_TARGET=Release
set TOOLCHAIN=../pjt/toolchain_windows.cmake

call build_with_cmake %PROJECT_FOLDER% %TEMP_FOLDER% %BUILD_TARGET% %TOOLCHAIN%
