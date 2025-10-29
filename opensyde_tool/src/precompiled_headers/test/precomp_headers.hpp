//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Precompiled header (header)

   To use this precompiled-header in a Qt project add the following to your .pro file:

   CONFIG   += precompile_header
   PRECOMPILED_HEADER = ../src/precomp_headers.h

   Contains a list of header files to be pre-compiled.
   see http://doc.qt.io/qt-5/qmake-precompiledheaders.html for details

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef  PRECOMP_HEADERS_TEST_HPP
#define  PRECOMP_HEADERS_TEST_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <winsock2.h>
// Adding windows.h here for avoiding problems with ambigous "byte" declarations
#include <windows.h>

//don't include anything here; so the test project can also verify whether all required headers are included

#endif
