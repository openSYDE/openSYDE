This folder contains the sources of open source libraries used with the openSYDE GUI that are linked 
with the GUI in binary form.

Building gettext and vector_blf
===============================
See the build instructions that come with each library on how to create the binary.

Building QCustomPlot
====================
To create the binary library to be linked with the openSYDE GUI create a static library from "qcustomplot.cpp".
Alternatively qcustomplot.cpp could be added to the project sources, but this will increase build times.

Building OpenSSL
================
1. Following https://github.com/openssl/openssl/blob/master/NOTES-WINDOWS.md#native-builds-using-mingw 
2. Install msys2 (necessary, see https://github.com/openssl/openssl/issues/6111 )
3. Open msys console
4. cd to openssl, e.g  execute: cd /c/Users/someguy/Documents/Qt_Creator/openssl/src/package_sources/
5. Execute: PATH=$PATH:/c/Qt/Qt5.15.2/Tools/mingw810_32/bin
6. Execute: ./Configure mingw no-shared no-sock no-stdio no-asm
(see https://stackoverflow.com/questions/50365513/building-a-static-version-of-openssl-library-using-md-switch )
7. Execute: mingw32-make
8. Setup result
8.1. cd to .. (toplevel)
8.2. Execute: mkdir .\result\openssl
8.3. Execute: mkdir .\result\openssl\include
8.4. Execute: mkdir .\result\openssl\include\crypto
8.5. Execute: mkdir .\result\openssl\include\internal
8.6. Execute: mkdir .\result\openssl\include\openssl
8.7. Execute: COPY .\src\package_sources\include\crypto\*.h .\result\openssl\include\crypto
8.8. Execute: COPY .\src\package_sources\include\internal\*.h .\result\openssl\include\internal
8.9. Execute: COPY .\src\package_sources\include\openssl\*.h .\result\openssl\include\openssl
8.10. Execute: COPY .\src\package_sources\*.a .\result\openssl