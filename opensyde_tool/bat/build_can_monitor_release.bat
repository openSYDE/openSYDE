rem build CAN Monitor application
del ..\temp_openSYDE_CAN_Monitor_Release\*.* /s /q
mkdir ..\temp_openSYDE_CAN_Monitor_Release
cd ..\temp_openSYDE_CAN_Monitor_Release
SET PATH=C:\Qt\Qt5.15.2\Tools\mingw810_32\bin\;%PATH%
SET PATH=C:\Qt\Qt5.15.2\5.15.2\mingw81_32\bin\;%PATH%

qmake.exe ../pjt/openSYDE_CAN_Monitor.pro -r -spec win32-g++ "CONFIG+=release"
if ERRORLEVEL 1 goto done

mingw32-make.exe clean
if ERRORLEVEL 1 goto done

rem output results to text file:
mingw32-make.exe -j4 2> mingw32diagnostics.txt
if ERRORLEVEL 1 goto done

cd ..\bat

:done