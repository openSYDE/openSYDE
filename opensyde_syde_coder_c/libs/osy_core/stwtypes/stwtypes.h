//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief      STW specific standard defines and types

   Standard typedefs in compliance with STW C++ Coding Rules.

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
//This header file is intended for 32bit compilation. If it is used with 64bit compilers the sized of the types will
// not be correct. This can result in undefined behavior in the application which might be hard to debug.
//So for convenience create a compilation fail on some commonly used 64bit compilers:
#if defined _WIN64 || defined __x86_64__ || defined __ppc64__
//If compilation fails here make sure a 32bit compiler is used
extern T_use_a_32bit_compiler gt_YesIWill; //portable compiler fail
#endif

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
#ifdef __cplusplus
namespace stw_types
{
#endif
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

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif
