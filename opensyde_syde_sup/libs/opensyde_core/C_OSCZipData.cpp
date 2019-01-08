//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       openSYDE Core zip/unzip utilities

   see header in .h file for details.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     01.09.2016  STW/A.Stangl (refactored from pre-existing code)
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "miniz.h"
#include "stwtypes.h"
#include "stwerrors.h"
#include "C_OSCZipData.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Zip data in memory

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

   \created     01.09.2016  STW/A.Stangl (refactored from pre-existing code)
*/
//-----------------------------------------------------------------------------
sint32 C_OSCZipData::h_Zip(uint8 * const opu8_Destination, uint32 & oru32_DestinationLength,
                           const uint8 * const opu8_Source, const uint32 ou32_SourceLength)
{
   sint32 s32_Return;

   s32_Return = compress(opu8_Destination, &oru32_DestinationLength, opu8_Source, ou32_SourceLength);
   if (s32_Return == Z_OK)
   {
      return C_NO_ERR;
   }
   return C_NOACT;
}

//-----------------------------------------------------------------------------
/*!
   \brief    Unzip data in memory

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

   \created     01.09.2016  STW/A.Stangl (refactored from pre-existing code)
*/
//-----------------------------------------------------------------------------
sint32 C_OSCZipData::h_Unzip(uint8 * const opu8_Destination, uint32 & oru32_DestinationLength,
                             const uint8 * const opu8_Source, const uint32 ou32_SourceLength)
{
   sint32 s32_Return;

   s32_Return = uncompress(opu8_Destination, &oru32_DestinationLength, opu8_Source, ou32_SourceLength);
   if (s32_Return == Z_OK)
   {
      return C_NO_ERR;
   }
   return C_NOACT;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get required size for compressed data

   Returns maximum buffer size required for compressed data.

   \param[in]     ou32_SourceLength   Size of uncompressed data in bytes

   \return
   Maximum size required for compressed data.

   \created     01.09.2016  STW/A.Stangl (refactored from pre-existing code)
*/
//-----------------------------------------------------------------------------
uint32 C_OSCZipData::h_GetRequiredBufSizeForZipping(const uint32 ou32_SourceLength)
{
   return compressBound(ou32_SourceLength);
}
