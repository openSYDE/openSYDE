This folder contains the sources of open source libraries used with the openSYDE GUI that are linked 
with the GUI in binary form.

Building gettext and vector_blf
===============================
See the build instructions that come with each library on how to create the binary.

Building QCustomPlot
====================
To create the binary library to be linked with the openSYDE GUI create a static library from "qcustomplot.cpp".
Alternativ3ely qcustomplot.cpp could be added to the project sources, but this will increase build times.