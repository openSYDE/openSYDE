//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief        MD5 hashing class

   ANSI C++ MD5 hashing class.

   Provides functions to calculate an MD5 over a BLOB of data or over a file.

   Loosely based on LTC_MD5 hash functions by Tom St Denis which are part of "libtomcrypt"
    provided as public domain at https://github.com/libtom/libtomcrypt (see 11.05.2020).
    Thanks Tom.

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef CMD5CHECKSUMH
#define CMD5CHECKSUMH

#include <cstdio>
#include "stwtypes.h"
#include "CSCLString.h"

#ifdef __BORLANDC__
#define PACKAGE __declspec(package)
#else
#define PACKAGE
#endif

namespace stw_md5
{
class PACKAGE CMD5Checksum
{
public:
   //according to the STW coding rules these functions should get a "h_" prefix
   //but we drop it for compatibility with existing applications
   static stw_scl::C_SCLString GetMD5(const stw_types::uint8 * const opu8_Data, const stw_types::uint32 ou32_Length);
   static stw_scl::C_SCLString GetMD5(std::FILE * const opt_File);
   static stw_scl::C_SCLString GetMD5(const stw_scl::C_SCLString & orc_FilePath);

protected:
private:
   struct C_HashState
   {
      stw_types::uint64 u64_Length;
      stw_types::uint32 au32_State[4];
      stw_types::uint32 u32_CurLen;
      stw_types::uint8 au8_Buffer[64];
   };

   static void mh_MD5Init(C_HashState * const opc_HashState);
   static stw_types::sint32 mh_MD5Process(C_HashState * const opc_HashState, const stw_types::uint8 * opu8_Input,
                                          stw_types::uint32 ou32_InputLength);
   static stw_types::sint32 mh_MD5Done(C_HashState * const opc_HashState, stw_types::uint8 * const opu8_Output);
   static void mh_MD5Compress(C_HashState * const opc_HashState, const stw_types::uint8 * const opu8_Buffer);
};
}

#endif
