//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       short description (header)

   To use this precompiled-header in a QtCreator project add the following to your .pro file:

   CONFIG   += precompile_header
   PRECOMPILED_HEADER = ../src/precomp_headers.h

   Contains a list of header files to be pre-compiled.
   see http://doc.qt.io/qt-5/qmake-precompiledheaders.html for details
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef PRECOMP_HEADERS_H
#define PRECOMP_HEADERS_H

//lint -esym(766,"precomp_headers.h")   effectively not used in lint "builds"; but that's exactly what we want
#ifndef _lint //speed up linting: don't include all of the headers for each linted .cpp file

/* -- Includes ------------------------------------------------------------- */

/* Add C includes here */

#if defined __cplusplus
/* Add C++ includes here */

#include <windows.h>
#include <cstdio>
#include <cstring>

#endif

#endif

#endif
