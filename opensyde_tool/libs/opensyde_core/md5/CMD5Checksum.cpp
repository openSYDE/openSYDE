//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       MD5 hashing class

   ANSI C++ MD5 hashing class.
   For details see documentation in .h file.

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h" //pre-compiled headers
#ifdef __BORLANDC__          //putting the pragmas in the config-header will not work
#pragma hdrstop
#pragma package(smart_init)
#endif

#include <cstring>
#include "stwtypes.h"
#include "stwerrors.h"
#include "CMD5Checksum.h"
#include "CSCLString.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_md5;
using namespace stw_scl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Preform MD5 compression

   \param[in,out]  opc_HashState   hash engine state
   \param[in]      opu8_Buffer     input buffer
*/
//----------------------------------------------------------------------------------------------------------------------
void CMD5Checksum::mh_md5_compress(C_HashState * const opc_HashState, const uint8 * opu8_Buffer)
{
#define LOAD32L(x, y)                            \
   x = ((uint32)((y)[3] & 255) << 24) | \
       ((uint32)((y)[2] & 255) << 16) | \
       ((uint32)((y)[1] & 255) << 8)  | \
       ((uint32)((y)[0] & 255));

#define F(x, y, z)  (z ^ (x & (y ^ z)))
#define G(x, y, z)  (y ^ (z & (y ^ x)))
#define H(x, y, z)  (x ^ y ^ z)
#define I(x, y, z)  (y ^ (x | (~z)))

#define ROLc(x, y) ((((uint32)(x) << (uint32)((y) & 31)) | \
                     (((uint32)(x) & 0xFFFFFFFFUL) >> (uint32)((32 - ((y) & 31)) & 31))) & \
                    0xFFFFFFFFUL)

#define FF(a, b, c, d, M, s, t) \
   a = (a + F(b, c, d) + M + t); a = ROLc(a, s) + b;

#define GG(a, b, c, d, M, s, t) \
   a = (a + G(b, c, d) + M + t); a = ROLc(a, s) + b;

#define HH(a, b, c, d, M, s, t) \
   a = (a + H(b, c, d) + M + t); a = ROLc(a, s) + b;

#define II(a, b, c, d, M, s, t) \
   a = (a + I(b, c, d) + M + t); a = ROLc(a, s) + b;

   uint32 au32_W[16];
   uint32 u32_A;
   uint32 u32_B;
   uint32 u32_C;
   uint32 u32_D;

   /* copy the state into 512-bits into W[0..15] */
   for (uint32 u32_I = 0; u32_I < 16; u32_I++)
   {
      LOAD32L(au32_W[u32_I], &opu8_Buffer[4 * u32_I]);
   }

   /* copy state */
   u32_A = opc_HashState->au32_State[0];
   u32_B = opc_HashState->au32_State[1];
   u32_C = opc_HashState->au32_State[2];
   u32_D = opc_HashState->au32_State[3];

   FF(u32_A, u32_B, u32_C, u32_D, au32_W[0], 7, 0xd76aa478UL)
   FF(u32_D, u32_A, u32_B, u32_C, au32_W[1], 12, 0xe8c7b756UL)
   FF(u32_C, u32_D, u32_A, u32_B, au32_W[2], 17, 0x242070dbUL)
   FF(u32_B, u32_C, u32_D, u32_A, au32_W[3], 22, 0xc1bdceeeUL)
   FF(u32_A, u32_B, u32_C, u32_D, au32_W[4], 7, 0xf57c0fafUL)
   FF(u32_D, u32_A, u32_B, u32_C, au32_W[5], 12, 0x4787c62aUL)
   FF(u32_C, u32_D, u32_A, u32_B, au32_W[6], 17, 0xa8304613UL)
   FF(u32_B, u32_C, u32_D, u32_A, au32_W[7], 22, 0xfd469501UL)
   FF(u32_A, u32_B, u32_C, u32_D, au32_W[8], 7, 0x698098d8UL)
   FF(u32_D, u32_A, u32_B, u32_C, au32_W[9], 12, 0x8b44f7afUL)
   FF(u32_C, u32_D, u32_A, u32_B, au32_W[10], 17, 0xffff5bb1UL)
   FF(u32_B, u32_C, u32_D, u32_A, au32_W[11], 22, 0x895cd7beUL)
   FF(u32_A, u32_B, u32_C, u32_D, au32_W[12], 7, 0x6b901122UL)
   FF(u32_D, u32_A, u32_B, u32_C, au32_W[13], 12, 0xfd987193UL)
   FF(u32_C, u32_D, u32_A, u32_B, au32_W[14], 17, 0xa679438eUL)
   FF(u32_B, u32_C, u32_D, u32_A, au32_W[15], 22, 0x49b40821UL)
   GG(u32_A, u32_B, u32_C, u32_D, au32_W[1], 5, 0xf61e2562UL)
   GG(u32_D, u32_A, u32_B, u32_C, au32_W[6], 9, 0xc040b340UL)
   GG(u32_C, u32_D, u32_A, u32_B, au32_W[11], 14, 0x265e5a51UL)
   GG(u32_B, u32_C, u32_D, u32_A, au32_W[0], 20, 0xe9b6c7aaUL)
   GG(u32_A, u32_B, u32_C, u32_D, au32_W[5], 5, 0xd62f105dUL)
   GG(u32_D, u32_A, u32_B, u32_C, au32_W[10], 9, 0x02441453UL)
   GG(u32_C, u32_D, u32_A, u32_B, au32_W[15], 14, 0xd8a1e681UL)
   GG(u32_B, u32_C, u32_D, u32_A, au32_W[4], 20, 0xe7d3fbc8UL)
   GG(u32_A, u32_B, u32_C, u32_D, au32_W[9], 5, 0x21e1cde6UL)
   GG(u32_D, u32_A, u32_B, u32_C, au32_W[14], 9, 0xc33707d6UL)
   GG(u32_C, u32_D, u32_A, u32_B, au32_W[3], 14, 0xf4d50d87UL)
   GG(u32_B, u32_C, u32_D, u32_A, au32_W[8], 20, 0x455a14edUL)
   GG(u32_A, u32_B, u32_C, u32_D, au32_W[13], 5, 0xa9e3e905UL)
   GG(u32_D, u32_A, u32_B, u32_C, au32_W[2], 9, 0xfcefa3f8UL)
   GG(u32_C, u32_D, u32_A, u32_B, au32_W[7], 14, 0x676f02d9UL)
   GG(u32_B, u32_C, u32_D, u32_A, au32_W[12], 20, 0x8d2a4c8aUL)
   HH(u32_A, u32_B, u32_C, u32_D, au32_W[5], 4, 0xfffa3942UL)
   HH(u32_D, u32_A, u32_B, u32_C, au32_W[8], 11, 0x8771f681UL)
   HH(u32_C, u32_D, u32_A, u32_B, au32_W[11], 16, 0x6d9d6122UL)
   HH(u32_B, u32_C, u32_D, u32_A, au32_W[14], 23, 0xfde5380cUL)
   HH(u32_A, u32_B, u32_C, u32_D, au32_W[1], 4, 0xa4beea44UL)
   HH(u32_D, u32_A, u32_B, u32_C, au32_W[4], 11, 0x4bdecfa9UL)
   HH(u32_C, u32_D, u32_A, u32_B, au32_W[7], 16, 0xf6bb4b60UL)
   HH(u32_B, u32_C, u32_D, u32_A, au32_W[10], 23, 0xbebfbc70UL)
   HH(u32_A, u32_B, u32_C, u32_D, au32_W[13], 4, 0x289b7ec6UL)
   HH(u32_D, u32_A, u32_B, u32_C, au32_W[0], 11, 0xeaa127faUL)
   HH(u32_C, u32_D, u32_A, u32_B, au32_W[3], 16, 0xd4ef3085UL)
   HH(u32_B, u32_C, u32_D, u32_A, au32_W[6], 23, 0x04881d05UL)
   HH(u32_A, u32_B, u32_C, u32_D, au32_W[9], 4, 0xd9d4d039UL)
   HH(u32_D, u32_A, u32_B, u32_C, au32_W[12], 11, 0xe6db99e5UL)
   HH(u32_C, u32_D, u32_A, u32_B, au32_W[15], 16, 0x1fa27cf8UL)
   HH(u32_B, u32_C, u32_D, u32_A, au32_W[2], 23, 0xc4ac5665UL)
   II(u32_A, u32_B, u32_C, u32_D, au32_W[0], 6, 0xf4292244UL)
   II(u32_D, u32_A, u32_B, u32_C, au32_W[7], 10, 0x432aff97UL)
   II(u32_C, u32_D, u32_A, u32_B, au32_W[14], 15, 0xab9423a7UL)
   II(u32_B, u32_C, u32_D, u32_A, au32_W[5], 21, 0xfc93a039UL)
   II(u32_A, u32_B, u32_C, u32_D, au32_W[12], 6, 0x655b59c3UL)
   II(u32_D, u32_A, u32_B, u32_C, au32_W[3], 10, 0x8f0ccc92UL)
   II(u32_C, u32_D, u32_A, u32_B, au32_W[10], 15, 0xffeff47dUL)
   II(u32_B, u32_C, u32_D, u32_A, au32_W[1], 21, 0x85845dd1UL)
   II(u32_A, u32_B, u32_C, u32_D, au32_W[8], 6, 0x6fa87e4fUL)
   II(u32_D, u32_A, u32_B, u32_C, au32_W[15], 10, 0xfe2ce6e0UL)
   II(u32_C, u32_D, u32_A, u32_B, au32_W[6], 15, 0xa3014314UL)
   II(u32_B, u32_C, u32_D, u32_A, au32_W[13], 21, 0x4e0811a1UL)
   II(u32_A, u32_B, u32_C, u32_D, au32_W[4], 6, 0xf7537e82UL)
   II(u32_D, u32_A, u32_B, u32_C, au32_W[11], 10, 0xbd3af235UL)
   II(u32_C, u32_D, u32_A, u32_B, au32_W[2], 15, 0x2ad7d2bbUL)
   II(u32_B, u32_C, u32_D, u32_A, au32_W[9], 21, 0xeb86d391UL)

   opc_HashState->au32_State[0] = opc_HashState->au32_State[0] + u32_A;
   opc_HashState->au32_State[1] = opc_HashState->au32_State[1] + u32_B;
   opc_HashState->au32_State[2] = opc_HashState->au32_State[2] + u32_C;
   opc_HashState->au32_State[3] = opc_HashState->au32_State[3] + u32_D;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize the hash state

   \param[in,out]  opc_HashState   hash engine state
*/
//----------------------------------------------------------------------------------------------------------------------
void CMD5Checksum::mh_md5_init(C_HashState * const opc_HashState)
{
   opc_HashState->au32_State[0] = 0x67452301UL;
   opc_HashState->au32_State[1] = 0xefcdab89UL;
   opc_HashState->au32_State[2] = 0x98badcfeUL;
   opc_HashState->au32_State[3] = 0x10325476UL;
   opc_HashState->u32_CurLen = 0;
   opc_HashState->u64_Length = 0;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Process a block of memory though the hash

   \param[in,out]  opc_HashState   hash engine state
   \param[in]    opu8_Input        The data to hash
   \param[in]    ou32_InputLength  The length of the data (bytes)

   \return
   C_NO_ERR  successful
   C_CONFIG  inconsistent hash engine status
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 CMD5Checksum::mh_md5_process(C_HashState * const opc_HashState, const uint8 * opu8_Input,
                                    uint32 ou32_InputLength)
{
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

   if (opc_HashState->u32_CurLen > sizeof(opc_HashState->au8_Buffer))
   {
      return C_CONFIG;
   }
   if ((opc_HashState->u64_Length + ou32_InputLength) < opc_HashState->u64_Length)
   {
      return C_CONFIG;
   }
   while (ou32_InputLength > 0)
   {
      if ((opc_HashState->u32_CurLen == 0) && (ou32_InputLength >= 64))
      {
         mh_md5_compress(opc_HashState, opu8_Input);
         opc_HashState->u64_Length += (64 * 8);
         opu8_Input             += 64;
         ou32_InputLength       -= 64;
      }
      else
      {
         uint32 u32_CurrentSize;
         u32_CurrentSize = MIN(ou32_InputLength, (64 - opc_HashState->u32_CurLen));
         memcpy(&opc_HashState->au8_Buffer[opc_HashState->u32_CurLen], opu8_Input, (size_t)u32_CurrentSize);
         opc_HashState->u32_CurLen += u32_CurrentSize;
         opu8_Input             += u32_CurrentSize;
         ou32_InputLength       -= u32_CurrentSize;
         if (opc_HashState->u32_CurLen == 64)
         {
            mh_md5_compress(opc_HashState, &opc_HashState->au8_Buffer[0]);
            opc_HashState->u64_Length += (8 * 64);
            opc_HashState->u32_CurLen = 0;
         }
      }
   }
   return C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Terminate the hash to get the digest

   \param[in,out]  opc_HashState   hash engine state
   \param[out]     opu8_Output     The destination of the hash (16 bytes)

   \return
   C_NO_ERR  successful
   C_CONFIG  inconsistent hash engine status
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 CMD5Checksum::mh_md5_done(C_HashState * const opc_HashState, uint8 * const opu8_Output)
{
#define STORE32L(x, y)                                                                     \
   (y)[3] = static_cast<uint8>(((x) >> 24) & 255); (y)[2] = static_cast<uint8>(((x) >> 16) & 255);   \
   (y)[1] = static_cast<uint8>(((x) >> 8) & 255); (y)[0] = static_cast<uint8>((x) & 255);

#define STORE64L(x, y)                                                                     \
   (y)[7] = static_cast<uint8>(((x) >> 56) & 255); (y)[6] = static_cast<uint8>(((x) >> 48) & 255);   \
   (y)[5] = static_cast<uint8>(((x) >> 40) & 255); (y)[4] = static_cast<uint8>(((x) >> 32) & 255);   \
   (y)[3] = static_cast<uint8>(((x) >> 24) & 255); (y)[2] = static_cast<uint8>(((x) >> 16) & 255);   \
   (y)[1] = static_cast<uint8>(((x) >> 8) & 255); (y)[0] = static_cast<uint8>((x) & 255);

   if (opc_HashState->u32_CurLen >= sizeof(opc_HashState->au8_Buffer))
   {
      return C_CONFIG;
   }

   /* increase the length of the message */
   opc_HashState->u64_Length += opc_HashState->u32_CurLen * 8;

   /* append the '1' bit */
   opc_HashState->au8_Buffer[opc_HashState->u32_CurLen++] = (unsigned char)0x80;

   /* if the length is currently above 56 bytes we append zeros
    * then compress.  Then we can fall back to padding zeros and length
    * encoding like normal.
    */
   if (opc_HashState->u32_CurLen > 56)
   {
      while (opc_HashState->u32_CurLen < 64)
      {
         opc_HashState->au8_Buffer[opc_HashState->u32_CurLen++] = 0;
      }
      mh_md5_compress(opc_HashState, opc_HashState->au8_Buffer);
      opc_HashState->u32_CurLen = 0;
   }

   /* pad upto 56 bytes of zeroes */
   while (opc_HashState->u32_CurLen < 56)
   {
      opc_HashState->au8_Buffer[opc_HashState->u32_CurLen++] = 0;
   }

   /* store length */
   STORE64L(opc_HashState->u64_Length, opc_HashState->au8_Buffer + 56);
   mh_md5_compress(opc_HashState, opc_HashState->au8_Buffer);

   /* copy output */
   for (uint8 u8_Word = 0; u8_Word < 4; u8_Word++)
   {
      STORE32L(opc_HashState->au32_State[u8_Word], &opu8_Output[4 * u8_Word]);
   }
   return C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Get MD5 of file.

   \param[in]  orc_FilePath   absolute or relative path of file

   \return
   Calculated MD5 (empty string if there are problems)
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString CMD5Checksum::GetMD5(const C_SCLString & orc_FilePath)
{
   //open the file as a binary file in readonly mode, denying write access
   FILE * pt_File;
   C_SCLString c_Return;

   pt_File = fopen(orc_FilePath.c_str(), "rb");
   if (pt_File == NULL)
   {
      return "";
   }

   //the file has been successfully opened, so now get and return its checksum
   c_Return = GetMD5(pt_File);
   fclose(pt_File);
   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Get MD5 of file.

   \param[in]  opt_File   C style file handle of file

   \return
   Calculated MD5 (empty string if there are problems)
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString CMD5Checksum::GetMD5(std::FILE * const opt_File)
{
   sintn sn_BlockSize;
   const uintn un_BufferSize = 4096; //checksum the file in blocks of 4096 bytes
   uint8 au8_Data[un_BufferSize];    //buffer for data read from the file
   C_SCLString c_StrMD5;
   C_HashState c_Hash;
   uint8 au8_Result[16];

   if (opt_File == NULL)
   {
      return "";
   }

   mh_md5_init(&c_Hash);

   //checksum the file in individual blocks
   while (1)
   {
      sn_BlockSize = fread(&au8_Data[0], 1, un_BufferSize, opt_File);
      if (sn_BlockSize <= 0)
      {
         break;
      }
      //we have data: process it
      mh_md5_process(&c_Hash, &au8_Data[0], sn_BlockSize);
   }

   mh_md5_done(&c_Hash, au8_Result);

   //Convert the hexadecimal checksum to a C_SCLString
   for (uint8 u8_Byte = 0U; u8_Byte < 16U; u8_Byte++)
   {
      C_SCLString c_Str;
      c_Str.PrintFormatted("%02x", au8_Result[u8_Byte]);
      c_StrMD5 += c_Str;
   }
   return c_StrMD5;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Get MD5 over data

   \param[in]  opu8_Data    data to calculate MD5 for
   \param[in]  ou32_Length  number of bytes referenced by opu8_Data

   \return
   Calculated MD5 (empty string if there are problems)
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString CMD5Checksum::GetMD5(const uint8 * const opu8_Data, const uint32 ou32_Length)
{
   //calculate and return the checksum
   C_SCLString c_StrMD5;
   C_HashState c_Hash;
   uint8 au8_Result[16];

   mh_md5_init(&c_Hash);
   mh_md5_process(&c_Hash, opu8_Data, ou32_Length);
   mh_md5_done(&c_Hash, au8_Result);

   //Convert the hexadecimal checksum to a C_SCLString
   for (uint8 u8_Byte = 0U; u8_Byte < 16U; u8_Byte++)
   {
      C_SCLString c_Str;
      c_Str.PrintFormatted("%02x", au8_Result[u8_Byte]);
      c_StrMD5 += c_Str;
   }
   return c_StrMD5;
}
