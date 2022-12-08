//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE Core zip/unzip utilities

   see header in .h file for details.

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#define MINIZ_NO_ZLIB_COMPATIBLE_NAMES //prevent namespace pollution
#include "miniz.h"
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_OscZipData.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Zip data in memory

   Compresses the specified data in memory.

   \param[out]    opu8_Destination         Destination buffer for compressed data. Must be at least oru32_DestLen bytes.
   \param[in,out] oru32_DestinationLength  in: size of opu8_Destination in bytes (must be >= the value returned by
                                            h_GetRequiredBufSizeForZipping)
                                           out: used number of bytes
   \param[in]     opu8_Source              source data to compress
   \param[in]     ou32_SourceLength        size of data referenced by opu8_Source in bytes

   \return
   C_NO_ERR   data compressed
   C_NOACT    error (e.g. destination buffer too small)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscZipData::h_Zip(uint8_t * const opu8_Destination, uint32_t & oru32_DestinationLength,
                            const uint8_t * const opu8_Source, const uint32_t ou32_SourceLength)
{
   int32_t s32_Return;
   //lint -e{8080} //using type provided by the library for compatibility
   mz_ulong x_DestinationLength = oru32_DestinationLength;

   s32_Return = mz_compress(opu8_Destination, &x_DestinationLength, opu8_Source, ou32_SourceLength);
   if (s32_Return == MZ_OK)
   {
      oru32_DestinationLength = x_DestinationLength;
      s32_Return = C_NO_ERR;
   }
   else
   {
      s32_Return = C_NOACT;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Unzip data in memory

   Uncompresses the specified data in memory.

   Knowing the size of the required destination buffer lies in the responsibility of the application.
   Typically the uncompressed size is stored along with the compressed data in some way (e.g. file header).

   \param[out]    opu8_Destination         Destination buffer
   \param[in,out] oru32_DestinationLength  in: size of destination buffer in bytes
                                           out: used size of destination buffer in bytes
   \param[in]     opu8_Source              Source buffer (compressed data)
   \param[in]     ou32_SourceLength        Size of source buffer in bytes

   \return
   C_NO_ERR   data uncompressed
   C_NOACT    error (e.g. destination buffer too small)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscZipData::h_Unzip(uint8_t * const opu8_Destination, uint32_t & oru32_DestinationLength,
                              const uint8_t * const opu8_Source, const uint32_t ou32_SourceLength)
{
   int32_t s32_Return;
   //lint -e{8080} //using type provided by the library for compatibility
   mz_ulong x_DestinationLength = oru32_DestinationLength;

   s32_Return = mz_uncompress(opu8_Destination, &x_DestinationLength, opu8_Source, ou32_SourceLength);
   if (s32_Return == MZ_OK)
   {
      oru32_DestinationLength = x_DestinationLength;
      s32_Return = C_NO_ERR;
   }
   else
   {
      s32_Return = C_NOACT;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get required size for compressed data

   Returns maximum buffer size required for compressed data.

   \param[in]     ou32_SourceLength   Size of uncompressed data in bytes

   \return
   Maximum size required for compressed data.
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_OscZipData::h_GetRequiredBufSizeForZipping(const uint32_t ou32_SourceLength)
{
   return mz_compressBound(ou32_SourceLength);
}
