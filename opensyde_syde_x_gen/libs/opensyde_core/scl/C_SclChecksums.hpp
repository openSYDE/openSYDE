//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief    Checksum calculation algorithms

   \class    stw::scl::C_SclChecksums
   \brief    Checksum calculation algorithms

   Provides implementations of various checksumming algorithms.
   For now only CRC routines.

   \copyright   Copyright 2015 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef CSCLCHECKSUMSHPP
#define CSCLCHECKSUMSHPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace scl
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SclChecksums
{
public:
   //"h_" prefix for static not used here for consistency with other SCL classes
   static void CalcCRC16(const void * const opv_Start, const uint32_t ou32_NumBytes, uint16_t & oru16_Crc);
   static void CalcCRC32(const void * const opv_Start, const uint32_t ou32_NumBytes, uint32_t & oru32_Crc);
   static int32_t CalcCRC32TriCore(const void * const opv_Start, const uint32_t ou32_NumBytes, uint32_t & oru32_Crc);
   static void CalcCRC16STW(const void * const opv_Start, const uint32_t ou32_NumBytes, uint16_t & oru16_Crc);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
}

#endif
