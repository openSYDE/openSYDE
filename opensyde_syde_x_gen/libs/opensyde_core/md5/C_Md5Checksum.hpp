//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief        MD5 hashing class

   ANSI C++ MD5 hashing class.

   Provides functions to calculate an MD5 over a BLOB of data or over a file.
   Technically not a "chechsum". Class name kept for compatibility.

   Loosely based on LTC_MD5 hash functions by Tom St Denis which are part of "libtomcrypt"
    provided as public domain at https://github.com/libtom/libtomcrypt (seen 11.05.2020).
    Thanks Tom.

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef CMD5CHECKSUMHPP
#define CMD5CHECKSUMHPP

#include <cstdio>
#include "stwtypes.hpp"
#include "C_SclString.hpp"

namespace stw
{
namespace md5
{
class C_Md5Checksum
{
public:
   //according to the STW coding rules these functions should get a "h_" prefix
   //but we drop it for compatibility with existing applications
   static stw::scl::C_SclString GetMD5(const uint8_t * const opu8_Data, const uint32_t ou32_Length);
   static stw::scl::C_SclString GetMD5(std::FILE * const opc_File);
   static stw::scl::C_SclString GetMD5(const stw::scl::C_SclString & orc_FilePath);

protected:
private:
   struct C_HashState
   {
      uint64_t u64_Length;
      uint32_t au32_State[4];
      uint32_t u32_CurLen;
      uint8_t au8_Buffer[64];
   };

   static void mh_Md5Init(C_HashState * const opc_HashState);
   static int32_t mh_Md5Process(C_HashState * const opc_HashState, const uint8_t * opu8_Input,
                                uint32_t ou32_InputLength);
   static int32_t mh_Md5Done(C_HashState * const opc_HashState, uint8_t * const opu8_Output);
   static void mh_Md5Compress(C_HashState * const opc_HashState, const uint8_t * const opu8_Buffer);
};
}
}

#endif
