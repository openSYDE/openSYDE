//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief    Checksum calculation algorithms

   \class    stw_scl::C_SCLChecksums
   \brief    Checksum calculation algorithms

   Provides implementations of various checksumming algorithms.
   For now only CRC routines.

   \copyright   Copyright 2015 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef CSCLCHECKSUMSH
#define CSCLCHECKSUMSH

/* -- Includes ------------------------------------------------------------- */
#include "stwtypes.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_scl
{
//maybe this will be a part of a Borland library:
#ifndef SCL_PACKAGE
#ifdef __BORLANDC__
#define SCL_PACKAGE __declspec(package)
#else
#define SCL_PACKAGE
#endif
#endif

/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */
class SCL_PACKAGE C_SCLChecksums
{
public:
   //"h_" prefix for static not used here for consistency with other SCL classes
   static void CalcCRC16(const void * const opv_Start, const stw_types::uint32 ou32_NumBytes,
                         stw_types::uint16 & oru16_CRC);
   static void CalcCRC32(const void * const opv_Start, const stw_types::uint32 ou32_NumBytes,
                         stw_types::uint32 & oru32_CRC);
   static stw_types::sint32 CalcCRC32TriCore(const void * const opv_Start, const stw_types::uint32 ou32_NumBytes,
                                             stw_types::uint32 & oru32_CRC);
   static void CalcCRC16STW(const void * const opv_Start, const stw_types::uint32 ou32_NumBytes,
                            stw_types::uint16 & oru16_CRC);
};

/* -- Extern Global Variables ---------------------------------------------- */
}

#endif
