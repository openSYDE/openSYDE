#include "precomp_headers.hpp" //pre-compiled headers
#ifdef __BORLANDC__            //putting the pragmas in the config-header will not work
#pragma hdrstop
#pragma package(smart_init)
#endif

#include "DiagLib_config.hpp" //diaglib configuration

#include <string.h>
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "DLLocalize.hpp"
#include "CXFLHexFile.hpp"
#include "C_SclString.hpp"
#include "TglUtils.hpp"
#include "C_SclChecksums.hpp"

using namespace stw::errors;
using namespace stw::hex_file;
using namespace stw::diag_lib;
using namespace stw::scl;
using namespace stw::tgl;

//-----------------------------------------------------------------------------
/*!
   \brief   convert error reported by CHexFile to textual representation

   \param[in]    ou32_ErrorCode      error code returned by CHexFile

   \return
   Textual representation of error.
*/
//-----------------------------------------------------------------------------
C_SclString C_XFLHexFile::ErrorCodeToErrorText(const uint32_t ou32_ErrorCode) const
{
   C_SclString c_Text;
   uint32_t u32_Address;

   switch (ou32_ErrorCode & ERR_MASK)
   {
   case WRN_NO_EOF_RECORD:
      c_Text = TGL_LoadStr(STR_FM_ERR_RD_HF_EOF);
      break;
   case WRN_RECORD_OVERLAY:
      u32_Address = this->GetLastOverlayErrorAddress(); //ou32_ErrorCode only contains lowest 7 nibbles !
      c_Text.PrintFormatted("Error in Hexfile: Address 0x%08x used twice !", u32_Address);
      break;
   case ERR_HEXLINE_SYNTAX:
      c_Text.PrintFormatted("%s %d", TGL_LoadStr(STR_FM_ERR_RD_HF_LINE_SYNTAX).c_str(), ou32_ErrorCode & 0x0FFFFFFFUL);
      break;
   case ERR_HEXLINE_CHECKSUM:
      c_Text.PrintFormatted("%s %d", TGL_LoadStr(STR_FM_ERR_RD_HF_LINE_CHECKSUM).c_str(),
                            ou32_ErrorCode & 0x0FFFFFFFUL);
      break;
   case ERR_HEXLINE_COMMAND:
      c_Text.PrintFormatted("%s %d", TGL_LoadStr(STR_FM_ERR_RD_HF_LINE_COMMAND).c_str(), ou32_ErrorCode & 0x0FFFFFFFUL);
      break;
   case ERR_NOT_ENOUGH_MEMORY:
      c_Text = TGL_LoadStr(STR_FM_ERR_RD_HF_MEMORY);
      break;
   case ERR_CANT_OPEN_FILE:
      c_Text = TGL_LoadStr(STR_FM_ERR_RD_HF_OPEN_FILE);
      break;
   default:
      c_Text = TGL_LoadStr(STR_FM_ERR_RD_HF_UNDEFINED);
      break;
   }
   return c_Text;
}

//-----------------------------------------------------------------------------
/*!
   \brief   get device information blocks from hex-file

   Extract all device information block information from hex-file

   \param[out]     orc_InfoBlocks            list of detected device information blocks
   \param[in]      ou32_SearchStartAddress   linear offset address to start search at
   \param[in]      oq_OnlyOneBlock           true -> read only one block then stop searching more
   \param[in]      oq_ExactAddressMatch      true -> info block must be at exactly ou32_SearchStartAddress
   \param[in]      oq_Block0Only             true -> only find "Block0" information blocks
                                                    These are the blocks with application information.
                                                    Other Blocks (1..9) can contain information about various libraries.
                                                    e.g. 1 = "BIOS".

   \return
   C_NO_ERR     everything OK (data in oat_InfoBlocks); but maybe there are 0 blocks
   C_NOACT      if oq_ExactAddressMatch: no block found at specified address
*/
//-----------------------------------------------------------------------------
int32_t C_XFLHexFile::GetECUInformationBlocks(C_SclDynamicArray<C_XFLECUInformation> & orc_InfoBlocks,
                                              const uint32_t ou32_SearchStartAddress, const bool oq_OnlyOneBlock,
                                              const bool oq_ExactAddressMatch, const bool oq_Block0Only)
{
   int32_t s32_Return;
   uint32_t u32_Address = ou32_SearchStartAddress;
   C_XFLECUInformation c_Block;
   uint16_t u16_Size = static_cast<uint16_t>(c_Block.GetMaxSizeOnECU());
   uint8_t * const pu8_Buffer = new uint8_t[u16_Size];
   uint16_t u16_Help;

   s32_Return = C_NO_ERR;
   char_t acn_Magic[XFL_DEVICE_INFO_MAGIC_LENGTH_V2];

   orc_InfoBlocks.SetLength(0);

   while (true)
   {
      //exact match required:
      //V1 and V2 have the same beginning:
      s32_Return = this->FindPattern(u32_Address, 5, reinterpret_cast<const uint8_t *>("Lx_?z"));
      if (s32_Return != C_NO_ERR)
      {
         break; //only stop if we cannot find any more pattern; in all other cases: continue searching
      }

      //get part of dump:
      u16_Help = XFL_DEVICE_INFO_MAGIC_LENGTH_V2;
      s32_Return = this->GetDataByAddress(u32_Address, u16_Help, reinterpret_cast<uint8_t *>(&acn_Magic[0]));
      if (s32_Return == C_NO_ERR)
      {
         s32_Return = C_CONFIG;
         //"Block0" ?
         if ((acn_Magic[6] == '.') || (oq_Block0Only == false))
         {
            //V1 ?
            if (acn_Magic[5] == '2')
            {
               s32_Return = C_NO_ERR;
               (void)memcpy(&c_Block.acn_Magic[0], &acn_Magic[0], XFL_DEVICE_INFO_MAGIC_LENGTH_V1);
            }
            //V2 or V3 ?
            else if ((acn_Magic[5] == 'g') && (acn_Magic[7] == '\0'))
            {
               s32_Return = C_NO_ERR;
               (void)memcpy(&c_Block.acn_Magic[0], &acn_Magic[0], XFL_DEVICE_INFO_MAGIC_LENGTH_V2);
            }
            else
            {
               //nothing we know about ...
            }
         }
      }

      if (oq_ExactAddressMatch == true)
      {
         if ((u32_Address != ou32_SearchStartAddress) || (s32_Return != C_NO_ERR))
         {
            //we searched the start address and did not find anything !
            delete[] pu8_Buffer;
            return C_NOACT;
         }
      }

      if (s32_Return == C_NO_ERR)
      {
         //for performance reasons we try to read the maximum possible size of the struct on the ECU:
         u16_Size = static_cast<uint16_t>(c_Block.GetMaxSizeOnECU());

         //the alignment on PC is not neccessarily the same as on the ECU
         //-> we have to use a temporary buffer to copy the data over ...
         (void)memset(pu8_Buffer, 0, u16_Size);
         s32_Return = this->GetDataByAddress(u32_Address, u16_Size, pu8_Buffer);
         switch (s32_Return)
         {
         case 0:
            break; //great ...
         case -2:  //data read but not fully
            s32_Return = 0;
            if (u16_Size < 2U) //we need at least 2 bytes for the header information
            {
               continue;
            }
            break;
         case -1:
         default:
            s32_Return = 0;
            continue; //nothing we can handle or undefined error -> continue
         }

         s32_Return = c_Block.ParseFromBLOB(pu8_Buffer, u16_Size);
         if (s32_Return == C_NO_ERR)
         {
            //we have all the data !
            //-> add to array
            orc_InfoBlocks.IncLength();
            orc_InfoBlocks[orc_InfoBlocks.GetHigh()] = c_Block;
            if (oq_OnlyOneBlock == true)
            {
               break;
            }
         }
      }
      u32_Address += XFL_DEVICE_INFO_MAGIC_LENGTH_V1; //done with this block ...
   }
   delete[] pu8_Buffer;
   return C_NO_ERR;
}

//-----------------------------------------------------------------------------
/*!
   \brief   calculate checksum over all data in the hexfile

   Calculate a 32bit checksum over all the bytes in the opened file.
   The checksum is calculated over address offsets and all bytes.
   So the sequence of hex-lines in the physical file will not affect the resulting checksum.
   If you need to check for a modified file, an MD5 checksum will be more helpful.
   The used algorithm is a CRC32 with a fixed start value.
   For a correct checksum we need to count in the address offsets and data bytes.
   We want the checksum to match for identical data. So we have to make sure to
   have a fixed concept of when to count in the address offset. Doing so at the
   beginning of a hex-line will not work, as then the checksum will change for a
   file with the same content but different record-lengths.
   So wo use the binary-block dump of the data and count in the address offset
    at the beginning of each block.

   \param[out]    oru32_Checksum   calculated checksum

   \return
   C_NO_ERR      checksum calculated
   C_CONFIG      error
*/
//-----------------------------------------------------------------------------
int32_t C_XFLHexFile::CalcFileChecksum(uint32_t & oru32_Checksum)
{
   uint32_t u32_Return;
   const C_HexDataDump * pc_Dump;

   pc_Dump = this->GetDataDump(u32_Return);
   if (u32_Return != NO_ERR)
   {
      return C_CONFIG;
   }

   oru32_Checksum = static_cast<uint32_t>(~0x56489437U); //fixed start value !
   for (uint32_t u32_Index = 0U; u32_Index < static_cast<uint32_t>(pc_Dump->at_Blocks.GetLength()); u32_Index++)
   {
      //address (serialize to make the code endian-safe):
      const uint32_t u32_AddressOffset = pc_Dump->at_Blocks[u32_Index].u32_AddressOffset;
      const uint8_t au8_AddressOffset[4] =
      {
         static_cast<uint8_t>(u32_AddressOffset),
         static_cast<uint8_t>(u32_AddressOffset >> 8U),
         static_cast<uint8_t>(u32_AddressOffset >> 16U),
         static_cast<uint8_t>(u32_AddressOffset >> 24U),
      };

      //address:
      C_SclChecksums::CalcCRC32(&au8_AddressOffset[0], 4U, oru32_Checksum);

      //data:
      C_SclChecksums::CalcCRC32(&pc_Dump->at_Blocks[u32_Index].au8_Data[0],
                                pc_Dump->at_Blocks[u32_Index].au8_Data.GetLength(), oru32_Checksum);
   }

   oru32_Checksum = ~oru32_Checksum;

   return C_NO_ERR;
}
