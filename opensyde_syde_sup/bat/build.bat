rem delete build folder and build using cmake
rem as a side effect this will also create the required configuration files for PC-lint
@echo off
SET CWD=%~dp0
if not exist ..\temp md ..\temp

rem clean folder ...
del /s /f /q ..\temp\*.* > nul

call build_incremental.bat
if errorlevel 1 exit /B