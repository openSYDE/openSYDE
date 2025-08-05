//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE Core zip/unzip utilities

   \class       stw::opensyde_core::C_OscZipData
   \brief       openSYDE Core zip/unzip utilities

   Wrapper for the zlib library.
   Provides functions to zip/unzip data in memory

   Typical usage:
   Zipping:
   - call h_GetRequiredBufSizeForZipping to get required buffer size
   - allocate buffer for compressed data
   - call h_Zip
   Unzipping:
   - call h_Unzip

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCZIPDATA_HPP
#define C_OSCZIPDATA_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_SclString.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscZipData
{
public:
   static int32_t h_Zip(uint8_t * const opu8_Destination, uint32_t & oru32_DestinationLength,
                        const uint8_t * const opu8_Source, const uint32_t ou32_SourceLength);
   static int32_t h_Unzip(uint8_t * const opu8_Destination, uint32_t & oru32_DestinationLength,
                          const uint8_t * const opu8_Source, const uint32_t ou32_SourceLength);
   static uint32_t h_GetRequiredBufSizeForZipping(const uint32_t ou32_SourceLength);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
}

#endif
