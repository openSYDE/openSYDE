//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief      STW specific standard defines and types

   Standard typedefs in compliance with STW C Coding Rules.

   Suitable for 32bit Windows applications compiled with:
   - Embarcadero C++ Builder
   - MinGW
   - MS Visual C++

   The definitions should also be fine for many other 32bit compilers and platforms.

   \copyright   Copyright 2007 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef STWTYPESH
#define STWTYPESH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
//Do *not* add a check to prevent building with a C++ compiler.
//C sources can be used in C++ application (but not the other way around).
//So we might have a scenario where a C++ application needs to include the header of a C module

/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

typedef unsigned char uint8;       ///< data type  8bit unsigned
typedef signed char sint8;         ///< data type  8bit signed
typedef unsigned short uint16;     ///< data type 16bit unsigned
typedef signed short sint16;       ///< data type 16bit signed
typedef unsigned long uint32;      ///< data type 32bit unsigned
typedef signed long sint32;        ///< data type 32bit signed
typedef unsigned long long uint64; ///< data type 64bit unsigned
typedef signed long long sint64;   ///< data type 64bit signed
typedef float float32;             ///< data type IEEE 32bit float
typedef double float64;            ///< data type IEEE 64bit float

// native data types
typedef unsigned int uintn; ///< data type native unsigned int
typedef signed int sintn;   ///< data type native signed int
typedef char charn;         ///< data type native char
#endif
