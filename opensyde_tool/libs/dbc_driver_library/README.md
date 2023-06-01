# Introduction

This is a library to access CAN Databases (aka CANdb, aka DBC files) from Vector Informatik.
It's optimized towards fast access on loaded data, not on load performance or save order.

# Build on Linux (e.g. Debian Testing)

Minimum requirements:

* compiler with C++14 support (gcc, clang, msvc)
* flex
* bison (>=3.3)

Building under Linux works as usual:

    mkdir build
    cd build
    cmake ..
    make
    make install DESTDIR=..
    make package

# Build on Windows (e.g. Windows 7 64-Bit)

Building under Windows contains the following steps:

* Use cmake-gui
* Set "Where is the source code:" to the root directory.
* Set "Where to build the binaries:" to folder "build" below the root directory.
* Configure and Generate
* Open the Visual Studio Solution (.sln) file in the build folder.
* Compile it in Release Configuration.

# Test

Static tests are

* Cppcheck (if OPTION_RUN_CPPCHECK is set)
* CCCC (if OPTION_RUN_CCCC is set)

Dynamic tests are

* Unit tests (if OPTION_RUN_TESTS is set)
* Example runs (if OPTION_RUN_EXAMPLES is set)
* Coverage (if OPTION_USE_LCOV_GCOV is set)

The test execution can be triggered using

    make test

# Package

The package generation can be triggered using

    make package

# Repository Structure

The following files are part of the source code distribution:

* src/_project_/
* src/_project_/tests/

The following files are working directories for building and testing:

* build/_project_/

The following files are products of installation and building:

* bin/
* lib/
* share/doc/_project_/
* share/man/
* include/_project_/

# Wanted features

* Templates/Functions for protocols (ARINC664, CANoe, CANopen, CAN_FD, CAPL, Ethernet, Interaction Layer, Network Management, ISO11783, J1587, J1939, MOST, NMEA)
* Comparation of two DBCs

# Missing test coverage

* Bit Timing (BS)
* Signal Types (SGTYPE)
* Signal Type Refs (SGTYPE)
* Signal Groups (SIG_GROUP)
* Signal Extended Value Types (SIG_VALTYPE)
* Extended Multiplexors (SG_MUL_VAL)
