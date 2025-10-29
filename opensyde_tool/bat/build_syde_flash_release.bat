rem build SYDEflash application

set PROJECT_FOLDER=..\pjt\SYDEflash
set TEMP_FOLDER=..\temp_SYDEflash_Release
set BUILD_TARGET=Release
set TOOLCHAIN=../pjt/toolchain_windows.cmake

call build_with_cmake %PROJECT_FOLDER% %TEMP_FOLDER% %BUILD_TARGET% %TOOLCHAIN%
