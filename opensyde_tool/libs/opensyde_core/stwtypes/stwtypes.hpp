//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief      STW specific standard defines and types

   Provide standard typedefs in compliance with STW C++ Coding Rules.
   When compiling with a version of C++11 or greater, then <cstdint> is dragged in to define the basic integer types.
   Otherwise <climits> is used to automagically define the types based on the sizes reported by <climits>.

   The header file is intended to be portable to different compilers.
   This should be compatible with most 32bit and 64bit compilers with all data models.

   Some preprocessor checks are in place to detect unknown compiler configurations.
   In such a case a compiler error will be triggered.

   Assumptions regarding float:
   float and double should be 32bit resp. 64bit on most relevant compilers.
   A portable sanity check is complex.
   If we ever will have a target where the assumption is not true then static check will report issues.

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef STWTYPESHPP
#define STWTYPESHPP

#ifndef __cplusplus
//use stwtypes.h for C sources
extern T_stwtypes_hpp_undefined_type gt_stwtypes_hpp_FailBuild;
#endif

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#if __cplusplus >= 201103L //>= C++11 ?
#include <cstdint>
#else
#include <climits>
#endif

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
#if __cplusplus >= 201103L //C++11 or later: use cstdint types
//It is not guaranteed that the types are placed in the global namespace but it is guaranteed they are in std.
//See 17.6.1.3/4 of C++11 standard (ISO/IEC 14882:2011)
//So we drag them into the global namespace to be sure they are available there on all targets.
//Conflicts are not to be expected: after all they *may* already be in the global namespace through cstdint.
//To make things easier we could directly include "stdint.h", but it is not guaranteed to be available in C++11.
using std::int8_t;
using std::uint8_t;
using std::int16_t;
using std::uint16_t;
using std::int32_t;
using std::uint32_t;
using std::int64_t;
using std::uint64_t;
#else //before C++11: typedef types ourselves

typedef signed char int8_t;
typedef unsigned char uint8_t;

#if (USHRT_MAX == 65535)
typedef signed short int16_t;
typedef unsigned short uint16_t;
#else
//stwtypes.hpp type for 16bit integer not known
extern T_stwtypes_hpp_undefined_type gt_stwtypes_hpp_FailBuild;
#endif

#if (INT_MAX == 2147483647)
typedef signed int int32_t;
typedef unsigned int uint32_t;
#elif (LONG_MAX == 2147483647)
typedef signed long int32_t;
typedef unsigned long uint32_t;
#else
//stwtypes.hpp type for 32bit integer not known
extern T_stwtypes_hpp_undefined_type gt_stwtypes_hpp_FailBuild;
#endif

//long long is only available from C++11 so we are in compiler specific terrain for 64bit values
#if defined _MSC_VER || defined __BORLANDC__
typedef signed __int64 int64_t;
typedef unsigned __int64 uint64_t;
#elif defined __GNUC__
typedef signed long long int64_t;
typedef unsigned long long uint64_t;
#else
//stwtypes.hpp type for 64bit integer not known
extern T_stwtypes_hpp_undefined_type gt_stwtypes_hpp_FailBuild;
#endif
#endif

typedef float float32_t;  ///< data type IEEE 32bit float
typedef double float64_t; ///< data type IEEE 64bit float
typedef char char_t;      ///< data type native char

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */

#endif
