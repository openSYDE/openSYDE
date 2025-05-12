//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief      STW specific standard defines and types

   Standard typedefs in compliance with STW C Coding Rules.
   The header file is intended to be portable to different compilers.
   This should be compatible with most 32bit and 64bit compilers with all data models.

   Assumptions regarding float:
   float and double should be 32bit resp. 64bit on most relevant compilers.
   A portable sanity check is complex.
   If we ever will have a target where the assumption is not true then static check will report issues.

   \copyright   Copyright 2007 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef STWTYPESH
#define STWTYPESH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <limits.h>

//Do *not* add a check to prevent building with a C++ compiler.
//C sources can be used in C++ application (but not the other way around).
//So we might have a scenario where a C++ application needs to include the header of a C module

/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

typedef signed char sint8;
typedef unsigned char uint8;

#if (USHRT_MAX == 65535)
typedef signed short sint16;
typedef unsigned short uint16;
#else
//stwtypes.h type for 16bit integer not known
extern T_stwtypes_h_undefined_type gt_stwtypes_h_FailBuild;
#endif

#if (INT_MAX == 2147483647)
typedef signed int sint32;
typedef unsigned int uint32;
#elif (LONG_MAX == 2147483647)
typedef signed long sint32;
typedef unsigned long uint32;
#else
//stwtypes.h type for 32bit integer not known
extern T_stwtypes_h_undefined_type gt_stwtypes_h_FailBuild;
#endif

//long long is only available from C11 so we are in compiler specific terrain for 64bit values
#if defined _MSC_VER || defined __BORLANDC__
typedef signed __int64 sint64;
typedef unsigned __int64 uint64;
#elif defined __GNUC__
typedef signed long long sint64;
typedef unsigned long long uint64;
#else
//stwtypes.h type for 64bit integer not known
extern T_stwtypes_h_undefined_type gt_stwtypes_h_FailBuild;
#endif

typedef float float32;  ///< data type IEEE 32bit float
typedef double float64; ///< data type IEEE 64bit float

// native data types
typedef unsigned int uintn; ///< data type native unsigned int
typedef signed int sintn;   ///< data type native signed int
typedef char charn;         ///< data type native char
#endif
