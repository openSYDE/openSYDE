//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE Core zip/unzip utilities

   \class       stw_opensyde_core::C_OSCZipData
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
#ifndef C_OSCZIPDATAH
#define C_OSCZIPDATAH

/* -- Includes ------------------------------------------------------------- */
#include "stwtypes.h"
#include "CSCLString.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OSCZipData
{
public:
   static stw_types::sint32 h_Zip(stw_types::uint8 * const opu8_Destination,
                                  stw_types::uint32 & oru32_DestinationLength,
                                  const stw_types::uint8 * const opu8_Source,
                                  const stw_types::uint32 ou32_SourceLength);
   static stw_types::sint32 h_Unzip(stw_types::uint8 * const opu8_Destination,
                                    stw_types::uint32 & oru32_DestinationLength,
                                    const stw_types::uint8 * const opu8_Source,
                                    const stw_types::uint32 ou32_SourceLength);
   static stw_types::uint32 h_GetRequiredBufSizeForZipping(const stw_types::uint32 ou32_SourceLength);
};

/* -- Extern Global Variables ---------------------------------------------- */
}

#endif
