//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Checksum calculation algorithms

   For details cf. documentation in .h file.

   \copyright   Copyright 2015 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h" //pre-compiled headers
#ifdef __BORLANDC__
#pragma hdrstop
#pragma package(smart_init)
#endif

#include "stwtypes.h"
#include "CSCLChecksums.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_scl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

//16bit CRC lookup table
static const uint16 mau16_CRC_TABLE[] =
{
   0x0000U, 0x1021U, 0x2042U, 0x3063U, 0x4084U, 0x50a5U, 0x60c6U, 0x70e7U,
   0x8108U, 0x9129U, 0xA14AU, 0xB16BU, 0xC18CU, 0xD1ADU, 0xE1CEU, 0xF1EFU,
   0x1231U, 0x0210U, 0x3273U, 0x2252U, 0x52B5U, 0x4294U, 0x72F7U, 0x62D6U,
   0x9339U, 0x8318U, 0xB37BU, 0xA35AU, 0xD3BDU, 0xC39CU, 0xF3FFU, 0xE3DEU,
   0x2462U, 0x3443U, 0x0420U, 0x1401U, 0x64E6U, 0x74C7U, 0x44A4U, 0x5485U,
   0xA56AU, 0xB54BU, 0x8528U, 0x9509U, 0xE5EEU, 0xF5CFU, 0xC5ACU, 0xD58DU,
   0x3653U, 0x2672U, 0x1611U, 0x0630U, 0x76D7U, 0x66F6U, 0x5695U, 0x46B4U,
   0xB75BU, 0xA77AU, 0x9719U, 0x8738U, 0xF7DFU, 0xE7FEU, 0xD79DU, 0xC7BCU,
   0x48C4U, 0x58E5U, 0x6886U, 0x78A7U, 0x0840U, 0x1861U, 0x2802U, 0x3823U,
   0xC9CCU, 0xD9EDU, 0xE98EU, 0xF9AFU, 0x8948U, 0x9969U, 0xA90AU, 0xB92BU,
   0x5AF5U, 0x4AD4U, 0x7AB7U, 0x6A96U, 0x1A71U, 0x0A50U, 0x3A33U, 0x2A12U,
   0xDBFDU, 0xCBDCU, 0xFBBFU, 0xEB9EU, 0x9B79U, 0x8B58U, 0xBB3BU, 0xAB1AU,
   0x6CA6U, 0x7C87U, 0x4CE4U, 0x5CC5U, 0x2C22U, 0x3C03U, 0x0C60U, 0x1C41U,
   0xEDAEU, 0xFD8FU, 0xCDECU, 0xDDCDU, 0xAD2AU, 0xBD0BU, 0x8D68U, 0x9D49U,
   0x7E97U, 0x6EB6U, 0x5ED5U, 0x4EF4U, 0x3E13U, 0x2E32U, 0x1E51U, 0x0E70U,
   0xFF9FU, 0xEFBEU, 0xDFDDU, 0xCFFCU, 0xBF1BU, 0xAF3AU, 0x9F59U, 0x8F78U,
   0x9188U, 0x81A9U, 0xB1CAU, 0xA1EBU, 0xD10CU, 0xC12DU, 0xF14EU, 0xE16FU,
   0x1080U, 0x00A1U, 0x30C2U, 0x20E3U, 0x5004U, 0x4025U, 0x7046U, 0x6067U,
   0x83B9U, 0x9398U, 0xA3FBU, 0xB3DAU, 0xC33DU, 0xD31CU, 0xE37FU, 0xF35EU,
   0x02B1U, 0x1290U, 0x22F3U, 0x32D2U, 0x4235U, 0x5214U, 0x6277U, 0x7256U,
   0xB5EAU, 0xA5CBU, 0x95A8U, 0x8589U, 0xF56EU, 0xE54FU, 0xD52CU, 0xC50DU,
   0x34E2U, 0x24C3U, 0x14A0U, 0x0481U, 0x7466U, 0x6447U, 0x5424U, 0x4405U,
   0xA7DBU, 0xB7FAU, 0x8799U, 0x97B8U, 0xE75FU, 0xF77EU, 0xC71DU, 0xD73CU,
   0x26D3U, 0x36F2U, 0x0691U, 0x16B0U, 0x6657U, 0x7676U, 0x4615U, 0x5634U,
   0xD94CU, 0xC96DU, 0xF90EU, 0xE92FU, 0x99C8U, 0x89E9U, 0xB98AU, 0xA9ABU,
   0x5844U, 0x4865U, 0x7806U, 0x6827U, 0x18C0U, 0x08E1U, 0x3882U, 0x28A3U,
   0xCB7DU, 0xDB5CU, 0xEB3FU, 0xFB1EU, 0x8BF9U, 0x9BD8U, 0xABBBU, 0xBB9AU,
   0x4A75U, 0x5A54U, 0x6A37U, 0x7A16U, 0x0AF1U, 0x1AD0U, 0x2AB3U, 0x3A92U,
   0xFD2EU, 0xED0FU, 0xDD6CU, 0xCD4DU, 0xBDAAU, 0xAD8BU, 0x9DE8U, 0x8DC9U,
   0x7C26U, 0x6C07U, 0x5C64U, 0x4C45U, 0x3CA2U, 0x2C83U, 0x1CE0U, 0x0CC1U,
   0xEF1FU, 0xFF3EU, 0xCF5DU, 0xDF7CU, 0xAF9BU, 0xBFBAU, 0x8FD9U, 0x9FF8U,
   0x6E17U, 0x7E36U, 0x4E55U, 0x5E74U, 0x2E93U, 0x3EB2U, 0x0ED1U, 0x1EF0U
};

//----------------------------------------------------------------------------------------------------------------------

//32bit CRC lookup table
static const uint32 mau32_CRC_TABLE[] =
{
   /* CRC polynomial 0xedb88320 */
   0x00000000U, 0x77073096U, 0xee0e612cU, 0x990951baU, 0x076dc419U, 0x706af48fU, 0xe963a535U, 0x9e6495a3U,
   0x0edb8832U, 0x79dcb8a4U, 0xe0d5e91eU, 0x97d2d988U, 0x09b64c2bU, 0x7eb17cbdU, 0xe7b82d07U, 0x90bf1d91U,
   0x1db71064U, 0x6ab020f2U, 0xf3b97148U, 0x84be41deU, 0x1adad47dU, 0x6ddde4ebU, 0xf4d4b551U, 0x83d385c7U,
   0x136c9856U, 0x646ba8c0U, 0xfd62f97aU, 0x8a65c9ecU, 0x14015c4fU, 0x63066cd9U, 0xfa0f3d63U, 0x8d080df5U,
   0x3b6e20c8U, 0x4c69105eU, 0xd56041e4U, 0xa2677172U, 0x3c03e4d1U, 0x4b04d447U, 0xd20d85fdU, 0xa50ab56bU,
   0x35b5a8faU, 0x42b2986cU, 0xdbbbc9d6U, 0xacbcf940U, 0x32d86ce3U, 0x45df5c75U, 0xdcd60dcfU, 0xabd13d59U,
   0x26d930acU, 0x51de003aU, 0xc8d75180U, 0xbfd06116U, 0x21b4f4b5U, 0x56b3c423U, 0xcfba9599U, 0xb8bda50fU,
   0x2802b89eU, 0x5f058808U, 0xc60cd9b2U, 0xb10be924U, 0x2f6f7c87U, 0x58684c11U, 0xc1611dabU, 0xb6662d3dU,
   0x76dc4190U, 0x01db7106U, 0x98d220bcU, 0xefd5102aU, 0x71b18589U, 0x06b6b51fU, 0x9fbfe4a5U, 0xe8b8d433U,
   0x7807c9a2U, 0x0f00f934U, 0x9609a88eU, 0xe10e9818U, 0x7f6a0dbbU, 0x086d3d2dU, 0x91646c97U, 0xe6635c01U,
   0x6b6b51f4U, 0x1c6c6162U, 0x856530d8U, 0xf262004eU, 0x6c0695edU, 0x1b01a57bU, 0x8208f4c1U, 0xf50fc457U,
   0x65b0d9c6U, 0x12b7e950U, 0x8bbeb8eaU, 0xfcb9887cU, 0x62dd1ddfU, 0x15da2d49U, 0x8cd37cf3U, 0xfbd44c65U,
   0x4db26158U, 0x3ab551ceU, 0xa3bc0074U, 0xd4bb30e2U, 0x4adfa541U, 0x3dd895d7U, 0xa4d1c46dU, 0xd3d6f4fbU,
   0x4369e96aU, 0x346ed9fcU, 0xad678846U, 0xda60b8d0U, 0x44042d73U, 0x33031de5U, 0xaa0a4c5fU, 0xdd0d7cc9U,
   0x5005713cU, 0x270241aaU, 0xbe0b1010U, 0xc90c2086U, 0x5768b525U, 0x206f85b3U, 0xb966d409U, 0xce61e49fU,
   0x5edef90eU, 0x29d9c998U, 0xb0d09822U, 0xc7d7a8b4U, 0x59b33d17U, 0x2eb40d81U, 0xb7bd5c3bU, 0xc0ba6cadU,
   0xedb88320U, 0x9abfb3b6U, 0x03b6e20cU, 0x74b1d29aU, 0xead54739U, 0x9dd277afU, 0x04db2615U, 0x73dc1683U,
   0xe3630b12U, 0x94643b84U, 0x0d6d6a3eU, 0x7a6a5aa8U, 0xe40ecf0bU, 0x9309ff9dU, 0x0a00ae27U, 0x7d079eb1U,
   0xf00f9344U, 0x8708a3d2U, 0x1e01f268U, 0x6906c2feU, 0xf762575dU, 0x806567cbU, 0x196c3671U, 0x6e6b06e7U,
   0xfed41b76U, 0x89d32be0U, 0x10da7a5aU, 0x67dd4accU, 0xf9b9df6fU, 0x8ebeeff9U, 0x17b7be43U, 0x60b08ed5U,
   0xd6d6a3e8U, 0xa1d1937eU, 0x38d8c2c4U, 0x4fdff252U, 0xd1bb67f1U, 0xa6bc5767U, 0x3fb506ddU, 0x48b2364bU,
   0xd80d2bdaU, 0xaf0a1b4cU, 0x36034af6U, 0x41047a60U, 0xdf60efc3U, 0xa867df55U, 0x316e8eefU, 0x4669be79U,
   0xcb61b38cU, 0xbc66831aU, 0x256fd2a0U, 0x5268e236U, 0xcc0c7795U, 0xbb0b4703U, 0x220216b9U, 0x5505262fU,
   0xc5ba3bbeU, 0xb2bd0b28U, 0x2bb45a92U, 0x5cb36a04U, 0xc2d7ffa7U, 0xb5d0cf31U, 0x2cd99e8bU, 0x5bdeae1dU,
   0x9b64c2b0U, 0xec63f226U, 0x756aa39cU, 0x026d930aU, 0x9c0906a9U, 0xeb0e363fU, 0x72076785U, 0x05005713U,
   0x95bf4a82U, 0xe2b87a14U, 0x7bb12baeU, 0x0cb61b38U, 0x92d28e9bU, 0xe5d5be0dU, 0x7cdcefb7U, 0x0bdbdf21U,
   0x86d3d2d4U, 0xf1d4e242U, 0x68ddb3f8U, 0x1fda836eU, 0x81be16cdU, 0xf6b9265bU, 0x6fb077e1U, 0x18b74777U,
   0x88085ae6U, 0xff0f6a70U, 0x66063bcaU, 0x11010b5cU, 0x8f659effU, 0xf862ae69U, 0x616bffd3U, 0x166ccf45U,
   0xa00ae278U, 0xd70dd2eeU, 0x4e048354U, 0x3903b3c2U, 0xa7672661U, 0xd06016f7U, 0x4969474dU, 0x3e6e77dbU,
   0xaed16a4aU, 0xd9d65adcU, 0x40df0b66U, 0x37d83bf0U, 0xa9bcae53U, 0xdebb9ec5U, 0x47b2cf7fU, 0x30b5ffe9U,
   0xbdbdf21cU, 0xcabac28aU, 0x53b39330U, 0x24b4a3a6U, 0xbad03605U, 0xcdd70693U, 0x54de5729U, 0x23d967bfU,
   0xb3667a2eU, 0xc4614ab8U, 0x5d681b02U, 0x2a6f2b94U, 0xb40bbe37U, 0xc30c8ea1U, 0x5a05df1bU, 0x2d02ef8dU
};

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Calculate 16 bit CRC STW style

   Calc CRC over specified area (as implemented in 16bit ESX classic ECUs)
   The start value can be influenced by setting oru16_CRC before calling the function.

   This algorithm does not perform the 16 final shift operations therefore it is not compatible with the
    regular 16bit CRC.

   \param[in]     opv_Start      data to calculate checksum over
   \param[in]     ou32_NumBytes  number of bytes to calculate the CRC for
   \param[in,out] oru16_CRC      start checksum / resulting checksum
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SCLChecksums::CalcCRC16STW(const void * const opv_Start, const uint32 ou32_NumBytes, uint16 & oru16_CRC)
{
   uint32 u32_Index;
   const uint8 * const pu8_Data = reinterpret_cast<const uint8 *>(opv_Start); //lint !e925 we need to parse byte-by-byte

   for (u32_Index = 0U; u32_Index < ou32_NumBytes; u32_Index++)
   {
      oru16_CRC = static_cast<uint16>(mau16_CRC_TABLE[oru16_CRC >> 8] ^ (static_cast<uint16>(oru16_CRC << 8)) ^
                                      pu8_Data[u32_Index]);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Calculate 16 bit CRC

   Calculates the 16-bit-CRC over ou32_NumBytes data bytes and places the result in oru16_CRC.
   The start value can be influenced by setting oru16_CRC before calling the function.

   Can be used for CCITT X25 CRC calculation by setting the start value to 0x1D0F.

   \param[in]     opv_Start      data to calculate checksum over
   \param[in]     ou32_NumBytes  number of bytes to calculate the CRC for
   \param[in,out] oru16_CRC      start checksum / resulting checksum
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SCLChecksums::CalcCRC16(const void * const opv_Start, const uint32 ou32_NumBytes, uint16 & oru16_CRC)
{
   uint32 u32_Index;
   uint8 u8_Index;
   const uint8 * const pu8_Data = reinterpret_cast<const uint8 *>(opv_Start); //lint !e925 we need to parse byte-by-byte

   for (u32_Index = 0U; u32_Index < ou32_NumBytes; u32_Index++)
   {
      u8_Index = static_cast<uint8>(oru16_CRC >> 8) ^ pu8_Data[u32_Index];
      oru16_CRC = static_cast<uint16>((static_cast<uint16>(oru16_CRC << 8)) ^ mau16_CRC_TABLE[u8_Index]);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Calculate 32 bit CRC

   Calculates the 32-bit-CRC over ou32_NumBytes data bytes and places the result in oru32_CRC.
   The start value can be influenced by setting oru32_CRC before calling the function.

   \param[in]     opv_Start      data to calculate checksum over
   \param[in]     ou32_NumBytes  number of bytes to calculate the CRC for
   \param[in,out] oru32_CRC      start checksum / resulting checksum
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SCLChecksums::CalcCRC32(const void * const opv_Start, const uint32 ou32_NumBytes, uint32 & oru32_CRC)
{
   const uint8 * const pu8_Data = reinterpret_cast<const uint8 *>(opv_Start); //lint !e925 we need to parse byte-by-byte
   uint32 u32_Index;

   for (u32_Index = 0U; u32_Index < ou32_NumBytes; u32_Index++)
   {
      oru32_CRC = (mau32_CRC_TABLE[((oru32_CRC) ^ (pu8_Data[u32_Index])) & 0xffU] ^ ((oru32_CRC) >> 8));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculate 32bit CRC as implemented in the TriCore Memory Checker Module

   This function calculates a checksum based on the hardware implementation of the
   TriCore Memory Checker Module. It implements the data compaction algorithm based
   on a Multiple Input Shift Register Implementation (MISR).

   The MISR memory check uses the IEEE 802.3 Standard Polynomal.

   The algorithm is only capable of consuming 32-Bit words, which depicts the
    limitation of the TriCore Hardware. Hence, the number of data bytes to be
    processed must be a multiple of 4.

   \param[in]        opv_Start            pointer to data that shall be included into the checksum
   \param[in]        ou32_NumBytes        number of data bytes the checksum shall be calculated for
   \param[in,out]    oru32_CRC            reference to the calculated checksum

   \return
   0          CRC calculated
   -1         ou32_NumBytes is not a multiple of 4
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SCLChecksums::CalcCRC32TriCore(const void * const opv_Start, const uint32 ou32_NumBytes, uint32 & oru32_CRC)
{
   sint32 s32_Return = 0;

   if ((ou32_NumBytes % 4) != 0)
   {
      s32_Return = -1;
   }
   else
   {
      uint32 u32_Tmp1;
      uint32 u32_Tmp2;
      uint32 u32_IndexBit;
      uint32 u32_IndexWord;
      uint32 u32_Value;
      const uint8 * const pu8_Data = reinterpret_cast<const uint8 *>(opv_Start); //lint !e925 we need to parse
                                                                                 // byte-by-byte
      const uint32 u32_Poly = 0xEDB88320UL;

      for (u32_IndexWord = 0U; u32_IndexWord < (ou32_NumBytes / 4U); u32_IndexWord++)
      {
         u32_Tmp1 = 0U;
         u32_Tmp2 = oru32_CRC & u32_Poly;

         for (u32_IndexBit = 0U; u32_IndexBit <= 31U; u32_IndexBit++)
         {
            u32_Tmp1 ^= ((u32_Tmp2 >> u32_IndexBit) & 1U);
         }

         u32_Value = static_cast<uint32>(pu8_Data[u32_IndexWord * 4]) +
                     ((static_cast<uint32>(pu8_Data[(u32_IndexWord * 4) + 1])) << 8) +
                     ((static_cast<uint32>(pu8_Data[(u32_IndexWord * 4) + 2])) << 16) +
                     ((static_cast<uint32>(pu8_Data[(u32_IndexWord * 4) + 3])) << 24);
         oru32_CRC = (u32_Value) ^ ((oru32_CRC << 1) | u32_Tmp1);
      }
   }

   return s32_Return;
}
