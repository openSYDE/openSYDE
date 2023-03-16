rem build GUI application
time /t
del ..\temp_openSYDE_Release\*.* /s /q
mkdir ..\temp_openSYDE_Release
cd ..\temp_openSYDE_Release
SET PATH=C:\Qt\Qt5.15.2\Tools\mingw810_32\bin\;%PATH%
SET PATH=C:\Qt\Qt5.15.2\5.15.2\mingw81_32\bin\;%PATH%

rem run qmake
qmake.exe ../pjt/openSYDE.pro -r -spec win32-g++ "CONFIG+=release"
if errorlevel 1 exit /b %errorlevel%

mingw32-make.exe clean
if errorlevel 1 exit /b %errorlevel%

rem perform actual build
rem output results to text file:
mingw32-make.exe -j4 2> mingw32diagnostics.txt
if errorlevel 1 exit /b %errorlevel%

cd ..\bat
time /t
