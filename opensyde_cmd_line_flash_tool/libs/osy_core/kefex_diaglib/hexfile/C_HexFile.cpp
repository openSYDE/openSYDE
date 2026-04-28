//************************************************************************
// .MODULE      load, sort and store Intel HEX files
//------------------------------------------------------------------------
// .FILE        C_HexFile.cpp
// .PROGRAM
// .GROUP
//------------------------------------------------------------------------
// .DESCRIPTION
//  - HexFile.LoadFromFile  loads a Intel HEX file into memory
//                          all records are sorted by address, Intel16 offset commands are
//                          converted into Intel32 offset commands
//  - HexFile.SaveToFile    stores the Intel HEX data from memory into file
//  - HexFile.LineInit      set data pointer to 1st line in memory
//  - HexFile.NextLine      returns pointer to HexLine (binary data)
//                           and set pointer to next line
//  - HexFile.NextLineString returns pointer to HexLine (C-string)
//                           and set pointer to next line
//  - HexFile.LineCount     returns number of stored Hex lines
//  - HexFile.MinAdr        returns min. address used by HEX file
//  - HexFile.MaxAdr        returns max. addres used by HEX file
//  - HexFile.Optimize      reorganize Intel HEX file with defined record length
//                          and optinal fill gaps
//  - HexFile.NextBinData   returns pointer to next HEX line binary data
//                          additionally it returns the data address offset
//                          and binary data size in byte
//************************************************************************
#include "precomp_headers.hpp" //pre-compiled headers

#include <cstring>
#include <cctype>

#include "C_HexFile.hpp"
#include "stwtypes.hpp"

//------------------------------------------------------------------------

using namespace stw::hex_file;

//------------------------------------------------------------------------

// hex file type
static const int32_t ms32_HEXFILE_ERR   =  -1; // GetFileType: file unknown
static const int32_t ms32_HEXFILE_IHEX  =   0; // GetFileType: file is intel-hex
static const int32_t ms32_HEXFILE_SREC  =   1; // GetFileType: file is s-record

static const uint32_t mu32_HEXBUFFER_SIZE  = 530U;     // buffer size for hex line data
static const int32_t ms32_INTELHEX_MAX    = (522 + 2); // max size of intel-hex line + \r\n

static const int32_t ms32_SRECORD_MAX  = 517; // max size of s-record line

static const uint8_t mu8_MIN_RECSIZE          =   8U;
static const uint8_t mu8_MAX_RECSIZE          =   255U;

// Intel HEX command
static const uint8_t mu8_CMD_DATA     =   0x00U;
static const uint8_t mu8_CMD_EOF      =   0x01U;
static const uint8_t mu8_CMD_XADR16   =   0x02U;
//static const uint8 mu8_CMD_START16  =   0x03U;
static const uint8_t mu8_CMD_XADR32   =   0x04U;
static const uint8_t mu8_CMD_START32  =   0x05U;

// Intel HEX data offset
static const uint8_t mu8_INTEL_LEN   =    0x00U;
static const uint8_t mu8_INTEL_ADRH  =    0x01U;
static const uint8_t mu8_INTEL_ADRL  =    0x02U;
static const uint8_t mu8_INTEL_CMD   =    0x03U;
static const uint8_t mu8_INTEL_DAT   =    0x04U;

// Motorola S-Record types
//static const uint8 mu8_SREC_HEADER  =   0U;
static const uint8_t mu8_SREC_DATA16  =   1U;
static const uint8_t mu8_SREC_END16   =   9U;
static const uint8_t mu8_SREC_DATA24  =   2U;
//static const uint8 mu8_SREC_END24   =   8U;
static const uint8_t mu8_SREC_DATA32  =   3U;
//static const uint8 mu8_SREC_END32   =   7U;
//static const uint8 mu8_SREC_COUNT   =   5U;

// Motorola S-Record data offset
static const uint8_t mu8_SREC_LEN     =   0x00U;
static const uint8_t mu8_SREC_ADR     =   0x01U;

static const uint32_t mu32_XADR32_MASK  =   0xFFFF0000U;

static const uint32_t mu32_MAX_DWORD    =   0xFFFFFFFFU;
static const uint32_t mu32_MIN_DWORD    =   0x00000000U;

//------------------------------------------------------------------------

C_HexFile::C_HexFile(void) :
   mu32_LastOverlayErrorAddress(0U)
{
   m_InitHexFile();
}

//------------------------------------------------------------------------

C_HexFile::~C_HexFile(void)
{
   this->Clear();
}

//------------------------------------------------------------------------

uint32_t C_HexFile::ByteCount(void) const
{
   return mu32_NumRawBytes;
}

//************************************************************************
// .FUNCTION    LoadFromFile
// .GROUP
//------------------------------------------------------------------------
// .DESCRIPTION
//  - HexFile.LoadFromFile load a HEX file into memory
//    internal hex format is intel-hex
//    all records are sorted by address, Intel16 offset commands are
//    converted into Intel32 offset commands
//------------------------------------------------------------------------
// .PARAMETERS  opcn_FileName      file name (C-string)
//------------------------------------------------------------------------
// .RETURNVALUE
//              NO_ERR                  0x00000000
//              WRN_NO_EOF_RECORD       0x10000000
//              ERR_HEXLINE_SYNTAX      0x8xxxxxxx
//              ERR_HEXLINE_CHECKSUM    0x9xxxxxxx
//              ERR_HEXLINE_COMMAND     0xAxxxxxxx
//              ERR_NOT_ENOUGH_MEMORY   0xExxxxxxx
//              ERR_CANT_OPEN_FILE      0xF0000000
//
//              xxxxxxx = error line number of input file
//************************************************************************
uint32_t C_HexFile::LoadFromFile(const char_t * const opcn_FileName)
{
   uint32_t u32_Error;
   int32_t s32_FileType;

   std::FILE * pc_File;

   this->Clear();
   pc_File = std::fopen(opcn_FileName, "rt");

   if (pc_File == NULL)
   {
      u32_Error = ERR_CANT_OPEN_FILE;
   }
   else
   {
      u32_Error = this->m_GetFileType(pc_File, s32_FileType); // get hex file type
      // intel-hex, s-record, ...
      if (u32_Error == NO_ERR)
      {
         u32_Error = m_AddHexLine(":020000040000FA\n"); // init data with zero offset

         if (u32_Error == NO_ERR)
         {
            switch (s32_FileType)
            {
            case ms32_HEXFILE_IHEX:
               u32_Error = m_LoadIntelHex(pc_File);
               break;
            case ms32_HEXFILE_SREC:
               u32_Error = m_LoadSRecord(pc_File);
               break;
            default:
               u32_Error = ERR_HEXLINE_SYNTAX;
               break;
            }

            (void)fclose(pc_File);

            if ((u32_Error & ERR_HEXLINE_SYNTAX) == 0U) // no error?
            {
               m_RemoveFirst(); // remove first element (zero offset)
            }
         }
         else
         {
            //Release created file pointer
            (void)fclose(pc_File);
         }
      }
      else
      {
         //Release created file pointer
         (void)fclose(pc_File);
      }
   }

   mq_DumpIsDirty = true;

   if (u32_Error == NO_ERR)
   {
      u32_Error = this->Validate();
   }
   return u32_Error;
}

//************************************************************************
// .FUNCTION    LoadIntelHex
// .GROUP
//------------------------------------------------------------------------
// .DESCRIPTION
//  - HexFile.LoadIntelHex load a Intel HEX file into memory
//    all records are sorted by address, Intel16 offset commands are
//    converted into Intel32 offset commands
//------------------------------------------------------------------------
// .PARAMETERS  opt_File                file handle
//------------------------------------------------------------------------
// .RETURNVALUE
//              NO_ERR                  0x00000000
//              WRN_NO_EOF_RECORD       0x10000000
//              ERR_HEXLINE_SYNTAX      0x8xxxxxxx
//              ERR_HEXLINE_CHECKSUM    0x9xxxxxxx
//              ERR_HEXLINE_COMMAND     0xAxxxxxxx
//              ERR_NOT_ENOUGH_MEMORY   0xExxxxxxx
//
//              xxxxxxx = error line number of input file
//************************************************************************
uint32_t C_HexFile::m_LoadIntelHex(std::FILE * const opt_File)
{
   uint32_t u32_Error = NO_ERR;
   char_t acn_HexBuffer[mu32_HEXBUFFER_SIZE];
   uint32_t u32_AdrNew;
   uint8_t u8_Command = mu8_CMD_DATA;
   uint8_t u8_Len = 0U;

   mu8_MaxRecordLength = 0U;

   while ((fgets(acn_HexBuffer, ms32_INTELHEX_MAX, opt_File)) != NULL)
   {
      mu32_LineCount++;

      u32_Error = m_GetIHexCommand(acn_HexBuffer, u8_Command);
      if (u32_Error == NO_ERR)
      {
         u32_Error = mh_GetByte(acn_HexBuffer, mu8_INTEL_LEN, u8_Len);
      }
      if (u32_Error != NO_ERR)
      {
         break;
      }

      if (u8_Len > mu8_MaxRecordLength)
      {
         mu8_MaxRecordLength = u8_Len;
      }

      switch (u8_Command)
      {
      case mu8_CMD_DATA:
         u32_Error = m_GetIntelAddress(acn_HexBuffer, u8_Command, u32_AdrNew);
         if (u32_Error == NO_ERR)
         {
            if ((mu32_AdrOffs & ~mu32_XADR32_MASK) != 0)
            {
               // change XADR16 into XADR32 records
               u32_Error = m_ConvRec16ToRec32(acn_HexBuffer);
            }
            else
            {
               m_SetDataPtr(u32_AdrNew);
               u32_Error = m_AddHexLine(acn_HexBuffer);
            }
            if (u32_Error == NO_ERR)
            {
               mu32_NumRawBytes += u8_Len;
            }
         }
         break;

      case mu8_CMD_XADR16:
      case mu8_CMD_XADR32:
         u32_Error = m_GetIntelAddress(acn_HexBuffer, u8_Command, u32_AdrNew);
         if (u32_Error == NO_ERR)
         {
            mu32_AdrOffs = u32_AdrNew;
            if (m_SetXAdrPtr(u32_AdrNew & mu32_XADR32_MASK) != 0U)
            {
               if (u8_Command == mu8_CMD_XADR16)
               {
                  // convert XADR16 into XADR32 offset
                  m_ConvOffs16To32(acn_HexBuffer);
               }

               u32_Error = m_AddHexLine(acn_HexBuffer);
            }
         }
         break;

      case mu8_CMD_EOF:
         m_SetEOFPtr();
         u32_Error = m_AddHexLine(acn_HexBuffer);
         break;

      default:
         break;
      }

      if ((u32_Error != 0U) || (u8_Command == mu8_CMD_EOF))
      {
         break;
      }
   }

   if ((u8_Command != mu8_CMD_EOF) && (u32_Error == NO_ERR)) // no EOF record?
   {
      m_SetEOFPtr(); // add EOF record
      u32_Error = m_AddHexLine(":00000001FF\n");
      if (u32_Error == 0U)
      {
         u32_Error = WRN_NO_EOF_RECORD; // set warning
      }
   }

   return u32_Error;
}

//************************************************************************
// .FUNCTION    LoadSRecord
// .GROUP
//------------------------------------------------------------------------
// .DESCRIPTION
//  - HexFile.LoadSRecord load a Motorola S_Record file into memory
//    convert records internal into intel hex format
//    all records are sorted by address, for address offset
//    Intel32 offset commands are used
//------------------------------------------------------------------------
// .PARAMETERS  opt_File                file handle
//------------------------------------------------------------------------
// .RETURNVALUE
//              NO_ERR                  0x00000000
//              ERR_HEXLINE_SYNTAX      0x8xxxxxxx
//              ERR_HEXLINE_CHECKSUM    0x9xxxxxxx
//              ERR_HEXLINE_COMMAND     0xAxxxxxxx
//              ERR_NOT_ENOUGH_MEMORY   0xExxxxxxx
//
//              xxxxxxx = error line number of input file
//************************************************************************
uint32_t C_HexFile::m_LoadSRecord(std::FILE * const opt_File)
{
   uint32_t u32_Error = NO_ERR;
   char_t acn_HexBuffer[mu32_HEXBUFFER_SIZE];
   char_t acn_IntelBuffer[mu32_HEXBUFFER_SIZE];
   uint32_t u32_AdrNew;
   uint8_t u8_RecordType;
   uint8_t u8_Length = 0U;

   mu8_MaxRecordLength = 0U;

   while ((fgets(acn_HexBuffer, ms32_SRECORD_MAX, opt_File)) != NULL)
   {
      mu32_LineCount++;

      u32_Error = m_GetSRecordType(acn_HexBuffer, u8_RecordType);
      if (u32_Error == NO_ERR)
      {
         u32_Error = mh_GetByte(acn_HexBuffer + 1, mu8_SREC_LEN, u8_Length);
         if (u32_Error != NO_ERR)
         {
            u32_Error |= (mu32_LineCount & ~ERR_MASK); // set line number info
         }
      }
      if (u32_Error == NO_ERR)
      {
         if (u8_Length > mu8_MaxRecordLength)
         {
            mu8_MaxRecordLength = u8_Length;
         }

         if ((u8_RecordType == mu8_SREC_DATA16) || (u8_RecordType == mu8_SREC_DATA24) ||
             (u8_RecordType == mu8_SREC_DATA32))
         {
            // get record address
            u32_Error = m_GetSRecordAddress(acn_HexBuffer, u8_RecordType, u32_AdrNew);

            if (mu32_AdrOffs != (u32_AdrNew & mu32_XADR32_MASK)) // 32bit address offset changed?
            {
               mu32_AdrOffs = (u32_AdrNew & mu32_XADR32_MASK); // yes -> set new address offset
               u32_Error = m_SetOffset(mu32_AdrOffs);          // set new offset record
            }

            if (u32_Error == NO_ERR) // no error?
            {
               // create intel hex record
               u8_Length -= static_cast<uint8_t>(u8_RecordType + 2U); // calc. intel-hex length
               acn_IntelBuffer[0] = ':';

               mu32_NumRawBytes += u8_Length;

               mh_SetByte(acn_IntelBuffer, mu8_INTEL_LEN, u8_Length);
               mh_SetWord(acn_IntelBuffer, mu8_INTEL_ADRH, (u32_AdrNew & ~mu32_XADR32_MASK));
               mh_SetByte(acn_IntelBuffer, mu8_INTEL_CMD, mu8_CMD_DATA);
               std::memmove(&acn_IntelBuffer[(mu8_INTEL_DAT * 2U) + 1U], &acn_HexBuffer[(u8_RecordType * 2U) + 6U],
                            static_cast<uint32_t>(u8_Length) * 2U);
               mh_CalcCheck(acn_IntelBuffer);

               m_SetDataPtr(u32_AdrNew);
               u32_Error = m_AddHexLine(acn_IntelBuffer);
            }
         }
      }

      if (u32_Error != NO_ERR)
      {
         break;
      }
   }

   if (u32_Error == NO_ERR) // no error?
   {
      m_SetEOFPtr(); // add EOF record to end of file
      u32_Error = m_AddHexLine(":00000001FF\n");
   }

   return u32_Error;
}

//************************************************************************
// .FUNCTION    GetFileType
// .GROUP
//------------------------------------------------------------------------
// .DESCRIPTION
//  - check if input file opt_File is intel-hex or motorola srecord
//------------------------------------------------------------------------
// .PARAMETERS  opt_File           file handle
//------------------------------------------------------------------------
// .RETURNVALUE
//              NO_ERR                  0x00000000
//              ERR_HEXLINE_SYNTAX      0x8xxxxxxx
//              ERR_HEXLINE_CHECKSUM    0x9xxxxxxx
//
//              xxxxxxx = error line number of input file
//************************************************************************
uint32_t C_HexFile::m_GetFileType(std::FILE * const opt_File, int32_t & ors32_FileType) const
{
   uint32_t u32_Error;
   const char_t * pcn_Ret;
   char_t acn_HexBuffer[mu32_HEXBUFFER_SIZE];
   uint8_t u8_Dummy;

   ors32_FileType = ms32_HEXFILE_ERR;                           // init type flag
   (void)std::memset(acn_HexBuffer, 0, mu32_HEXBUFFER_SIZE);    // clear hex buffer
   pcn_Ret = fgets(acn_HexBuffer, ms32_INTELHEX_MAX, opt_File); // read first line of file
   rewind(opt_File);                                            // set file pointer to start of file

   if (pcn_Ret != acn_HexBuffer)
   {
      // fgets failed
      u32_Error = ERR_HEXLINE_SYNTAX;
   }
   else if (acn_HexBuffer[0] == ':') // line might be intel-hex?
   {
      u32_Error = m_GetIHexCommand(acn_HexBuffer, u8_Dummy);
      if (u32_Error == NO_ERR) // check hexline
      {
         ors32_FileType = ms32_HEXFILE_IHEX;
      }
   }
   else if (acn_HexBuffer[0] == 'S') // line might be s-record?
   {
      u32_Error = m_GetSRecordType(acn_HexBuffer, u8_Dummy);
      if (u32_Error == NO_ERR) // check hexline
      {
         ors32_FileType = ms32_HEXFILE_SREC;
      }
   }
   else
   {
      u32_Error = ERR_HEXLINE_SYNTAX; // init error flag
   }

   return u32_Error;
}

//************************************************************************
// .FUNCTION    Optimize
// .GROUP
//------------------------------------------------------------------------
// .DESCRIPTION
//  - HexFile.Optimize Intel HEX file into memory
//    all records are sorted by address, produce default record size iRecSize
//    fill gaps with bFillPattern, if iFillFlag == true
//  - Note: This approach requires to create a memory dump of all the hex file data
//           including all gaps. So if using this function on hex files with big gap
//           (e.g. caused by virtual address offsets) this function can easily run out
//           of memory.
//------------------------------------------------------------------------
// .PARAMETERS  ou32_RecSize:  default record size 8..255
//              os32_FillFlag: true -> fill gaps with bFillPattern
//------------------------------------------------------------------------
// .RETURNVALUE
//              NO_ERR                  0x00000000
//              WRN_RECORD_OVERLAY      0x2xxxxxxx (xxxxxxx = lowest 7 nibbles of the offending address)
//              ERR_NOT_ENOUGH_MEMORY   0xE0000000
//              ERR_NO_DATA             0xB0000000
//************************************************************************
uint32_t C_HexFile::OptimizeLinear(const uint32_t ou32_RecSize, const int32_t os32_FillFlag,
                                   const uint8_t ou8_FillPattern)
{
   uint32_t u32_Error = NO_ERR;

   if (LineInit() != NULL) // HEX Data loaded?
   {
      uint32_t u32_Offset = mu32_MinAdr;
      uint32_t u32_Size = (mu32_MaxAdr - mu32_MinAdr) + 1U;
      uint16_t * pu16_BinImage;

      try // try to allocate memory
      {
         pu16_BinImage = new uint16_t[u32_Size]; // allocate space for memory image
      }
      catch (...) // not enough memory?
      {
         u32_Error = ERR_NOT_ENOUGH_MEMORY;
      }

      if (u32_Error == NO_ERR) // memory OK?
      {
         (void)std::memset(pu16_BinImage, 0, u32_Size * sizeof(uint16_t)); // clear memory
         // store memory image
         uint32_t u32_Warning = m_CopyHex2Mem(pu16_BinImage, u32_Offset);

         if (os32_FillFlag != 0) // fill gaps?
         {
            for (uint32_t u32_ByteIndex = 0U; u32_ByteIndex < u32_Size; u32_ByteIndex++) // yes, fill unused data
            {
               if ((pu16_BinImage[u32_ByteIndex] & 0xFF00U) == 0U)
               {
                  pu16_BinImage[u32_ByteIndex] =
                     static_cast<uint16_t>(static_cast<uint16_t>(0x0100U) + ou8_FillPattern);
               }
            }
         }

         u32_Error = CreateHexFile(pu16_BinImage, u32_Offset, u32_Size, ou32_RecSize); // create new HEX file from image

         if (u32_Error == 0U) // if no error
         {
            u32_Error = u32_Warning; // return warning
         }

         delete[] pu16_BinImage; // delete memory image
      }
   }

   return u32_Error;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Rearrange hex file records in memory

   The existing hex file data is rearranged in memory:
   The record sizes are changed to new lengths.

   Note: does not create a linear memory dump including gaps. Instead GetDataDump() is used
    to create an image only containing the used areas.
    Therefore memory consumption is not affected by gaps in the hex file data.

   \param[in]  ou32_RecSize     new default hex record size (8..255)

   \return
   NO_ERR                 -> no problems
   WRN_RECORD_OVERLAY     -> address used twice (lowest 7 nibbles contain lowest 7 nibbles of the offending address)
   ERR_NOT_ENOUGH_MEMORY  -> PC out of memory
   ERR_NO_DATA            -> no hex file data available
*/
//-----------------------------------------------------------------------------
uint32_t C_HexFile::Optimize(const uint32_t ou32_RecSize)
{
   uint32_t u32_Error = ERR_NO_DATA;
   const C_HexDataDump * pc_Dump;

   if (this->LineInit() != NULL) // HEX Data loaded?
   {
      pc_Dump = this->GetDataDump(u32_Error);
      if (u32_Error == NO_ERR)
      {
         u32_Error = this->CreateHexFile(*pc_Dump, ou32_RecSize); // create new HEX file from image
      }
   }

   return u32_Error;
}

//************************************************************************
// .FUNCTION    SaveToFile
// .GROUP
//------------------------------------------------------------------------
// .DESCRIPTION
//  - HexFile.SaveToFile stores the Intel HEX data from memory into file
//------------------------------------------------------------------------
// .PARAMETERS  opcn_FileName    file name (C-string)
//------------------------------------------------------------------------
// .RETURNVALUE
//              NO_ERR                  0x00000000
//              ERR_CANT_OPEN_FILE      0xF0000000
//************************************************************************
uint32_t C_HexFile::SaveToFile(const char_t * const opcn_FileName)
{
   uint32_t u32_Error = NO_ERR;
   const uint8_t  * pu8_HexLine;

   std::FILE * pc_File;

   if (LineInit() != NULL) // HEX Data loaded?
   {
      // write binary file to avoid OS-specific handling of newlines
      pc_File = std::fopen(opcn_FileName, "wb"); // yes, now open acFileName


      if (pc_File == NULL)
      {
         u32_Error = ERR_CANT_OPEN_FILE; // something is wrong with acFileName!
      }
      else
      {
         // are there any lines left?
         while ((pu8_HexLine = NextLine()) != NULL)
         {
            fputs(mh_HexLineString(pu8_HexLine), pc_File); // write string into file
            // we always create CR/LF to avoid different hex-file data on different OS
            fputs("\r\n", pc_File);

         }

         (void)fclose(pc_File);
      }
   }

   return (u32_Error);
}

//************************************************************************
// .FUNCTION    NextLineString
// .GROUP
//------------------------------------------------------------------------
// .DESCRIPTION
//  - HexFile.NextLineString returns next HEX line as C-String
//------------------------------------------------------------------------
// .PARAMETERS  none
//------------------------------------------------------------------------
// .RETURNVALUE
//              NULL if no data available
//************************************************************************
const char_t * C_HexFile::NextLineString(void)
{
   const char_t * pcn_String = NULL;
   const uint8_t * pu8_HexLine = NextLine();

   if (pu8_HexLine != NULL)
   {
      pcn_String = mh_HexLineString(pu8_HexLine);
   }

   return pcn_String;
}

//************************************************************************
// .FUNCTION    NextBinData
// .GROUP
//------------------------------------------------------------------------
// .DESCRIPTION
//  - HexFile.NextBinData returns pointer to next HEX line binary data
//                        additionally it returns the 32bit data address
//                        and binary data size in byte
//------------------------------------------------------------------------
// .PARAMETERS  none
//------------------------------------------------------------------------
// .RETURNVALUE pointer to binary data
//              NULL if no data available
//              oru32_Address returns 32bit data address
//              oru8_Size returns number of data bytes
//************************************************************************
const uint8_t * C_HexFile::NextBinData(uint32_t & oru32_Address, uint8_t & oru8_Size)
{
   const uint8_t * pu8_Line;
   T_HexLine * pt_Line;

   oru32_Address = 0; // clear return values
   oru8_Size = 0;

   while ((pu8_Line = NextLine()) != NULL) // search for next data line
   {
      if (pu8_Line[mu8_INTEL_CMD] == mu8_CMD_DATA)
      {
         pt_Line = mpt_HexData->pt_Prev;
         oru32_Address = pt_Line->u32_XAdr;   // get 32bit line address
         oru8_Size = pu8_Line[mu8_INTEL_LEN]; // get data len
         pu8_Line = &pu8_Line[mu8_INTEL_DAT]; // get data pointer
         break;
      }
   }

   return pu8_Line;
}

//************************************************************************
// .FUNCTION    InitHexFile
// .GROUP
//------------------------------------------------------------------------
// .DESCRIPTION
//    InitHexFile initialize class internal variables
//    called by constructor and ClearHexFile
//------------------------------------------------------------------------
// .PARAMETERS  void
//------------------------------------------------------------------------
// .RETURNVALUE void
//************************************************************************
void C_HexFile::m_InitHexFile(void)
{
   mu32_MinAdr = mu32_MAX_DWORD;
   mu32_MaxAdr = mu32_MIN_DWORD;
   mu32_AdrOffs = 0U;
   mu32_LineCount = 0U;
   mu32_LineCountNew = 0U;
   mu8_MaxRecordLength = 0U;
   mu32_NumRawBytes = 0U;
   mu32_LastOverlayErrorAddress = 0U;
   mpt_DataEntry = NULL;
   mpt_HexData = NULL;
   mpt_Prev = NULL;
   mq_DumpIsDirty = true;
}

//************************************************************************
// .FUNCTION    ClearHexFile
// .GROUP
//------------------------------------------------------------------------
// .DESCRIPTION
//    Clear frees all dynamic allocated memory
//    called by destructor and at the beginning of LoadFromFile
//------------------------------------------------------------------------
// .PARAMETERS  void
//------------------------------------------------------------------------
// .RETURNVALUE void
//************************************************************************
void C_HexFile::Clear(void)
{
   const T_HexLine * pt_HexLine;
   const T_HexLine * pt_NextLine;

   mu32_NumRawBytes = 0U;

   if (mpt_DataEntry != NULL)
   {
      pt_HexLine = mpt_DataEntry;

      do
      {
         pt_NextLine = pt_HexLine->pt_Next;

         if (pt_HexLine->pu8_HexLine != NULL)
         {
            delete[] pt_HexLine->pu8_HexLine;
         }

         delete pt_HexLine;

         pt_HexLine = pt_NextLine;
      }
      while (pt_HexLine != mpt_DataEntry);
   }

   m_InitHexFile();
}

//************************************************************************
// .FUNCTION    CopyHex2Mem
// .GROUP
//------------------------------------------------------------------------
// .DESCRIPTION
//    copy HEX file data to memory image
//------------------------------------------------------------------------
// .PARAMETERS  opu16_BinImage:  pointer to image memory
//              ou32_Offset:    zero offset for BinImage
//------------------------------------------------------------------------
// .RETURNVALUE
//              NO_ERR                  0x00000000
//              WRN_RECORD_OVERLAY      0x2xxxxxxx
//
//              xxxxxxx = lowest 7 nibbles of the offending address
//************************************************************************
uint32_t C_HexFile::m_CopyHex2Mem(uint16_t * const opu16_BinImage, const uint32_t ou32_Offset)
{
   uint32_t u32_Error = NO_ERR;
   uint32_t u32_AdrOffset = 0U;
   const uint8_t * pu8_HexLine;

   if (LineInit() != NULL) // HEX Data loaded?
   {
      while ((pu8_HexLine = NextLine()) != NULL)
      {
         uint8_t u8_Command = pu8_HexLine[mu8_INTEL_CMD];

         if (u8_Command == mu8_CMD_EOF) // EOF record?
         {
            break;
         }

         if (u8_Command == mu8_CMD_XADR32) // address offset?
         {
            u32_AdrOffset =  (static_cast<uint32_t>(pu8_HexLine[mu8_INTEL_DAT]) << 24U);
            u32_AdrOffset |= (static_cast<uint32_t>(pu8_HexLine[mu8_INTEL_DAT + 1]) << 16U);
         }
         else if (u8_Command == mu8_CMD_DATA) // data record?
         {
            uint32_t u32_NumBytes = pu8_HexLine[mu8_INTEL_LEN];
            uint32_t u32_Adr = (static_cast<uint32_t>(pu8_HexLine[mu8_INTEL_ADRH]) << 8U);
            u32_Adr |= pu8_HexLine[mu8_INTEL_ADRL];
            u32_Adr = (u32_Adr + u32_AdrOffset) - ou32_Offset;

            for (uint32_t u32_ByteIndex = 0; u32_ByteIndex < u32_NumBytes; u32_ByteIndex++) // copy data into memory
            {
               // check for data overlay
               if (((opu16_BinImage[u32_Adr + u32_ByteIndex] & 0xFF00U) != 0) && (u32_Error == NO_ERR))
               {
                  // -> set warning
                  u32_Error = (WRN_RECORD_OVERLAY | ((u32_Adr + ou32_Offset + u32_ByteIndex) & ~ERR_MASK));
                  mu32_LastOverlayErrorAddress = u32_Adr + ou32_Offset + u32_ByteIndex;
               }
               opu16_BinImage[u32_Adr + u32_ByteIndex] = static_cast<uint16_t>(static_cast<uint16_t>(0x0100U) +
                                                                               pu8_HexLine[mu8_INTEL_DAT +
                                                                                           u32_ByteIndex]);
            }
         }
         else
         {
            //nothing we know about ...
         }
      }
   }

   return u32_Error;
}

//************************************************************************
// .FUNCTION    CreateHexFile
// .GROUP
//------------------------------------------------------------------------
// .DESCRIPTION
//  Create hex file data from binary memory image
//------------------------------------------------------------------------
// .PARAMETERS  opu16_BinImage:  pointer to image memory
//              ou32_Offset:     address offset for BinImage
//              ou32_Size:       size of BinImage
//              ou32_RecSize:    default record size 8..255
//------------------------------------------------------------------------
// .RETURNVALUE
//              NO_ERR                  0x00000000
//              ERR_NOT_ENOUGH_MEMORY   0xExxxxxxx
//
//              xxxxxxx = line number of optimized file
//************************************************************************
uint32_t C_HexFile::CreateHexFile(const uint16_t * const opu16_BinImage, const uint32_t ou32_Offset,
                                  const uint32_t ou32_Size, const uint32_t ou32_RecSize)
{
   uint32_t u32_Error;
   uint32_t u32_Adr;
   uint32_t u32_RecSize;
   char_t acn_Record[mu32_HEXBUFFER_SIZE];
   int32_t s32_Length = 0;
   bool q_RecordOpen = false;

   mq_DumpIsDirty = true;

   u32_RecSize = ou32_RecSize;
   if (u32_RecSize < mu8_MIN_RECSIZE)
   {
      u32_RecSize = mu8_MIN_RECSIZE;
   }
   if (u32_RecSize > mu8_MAX_RECSIZE)
   {
      u32_RecSize = mu8_MAX_RECSIZE;
   }

   this->Clear(); // clear existing hex file data

   mu32_NumRawBytes = ou32_Size;

   u32_Error = m_AddHexLine(":020000040000FA\n"); // init data with zero offset

   if (u32_Error == NO_ERR)
   {
      for (uint32_t u32_ByteIndex = 0; u32_ByteIndex < ou32_Size; u32_ByteIndex++)
      {
         while ((opu16_BinImage[u32_ByteIndex] & 0xFF00U) != 0U) // do while data valid...
         {
            u32_Adr = ou32_Offset + u32_ByteIndex; // create full address

            if ((u32_Adr & mu32_XADR32_MASK) != mu32_AdrOffs)
            {
               u32_Error = m_CloseRecord(acn_Record, q_RecordOpen);
               if (u32_Error == NO_ERR)
               {
                  // create extended 32bit address record
                  mu32_AdrOffs = (u32_Adr & mu32_XADR32_MASK);
                  u32_Error = m_SetOffset(mu32_AdrOffs);
               }
            }

            if (u32_Error == NO_ERR)
            {
               if (((u32_Adr - mu32_AdrOffs) % u32_RecSize) == 0U) // record size boundary?
               {
                  u32_Error = m_CloseRecord(acn_Record, q_RecordOpen);
               }
            }
            if (u32_Error != NO_ERR)
            {
               break;
            }

            if (q_RecordOpen == false) // no record open?
            {
               // -> open data record...
               s32_Length = 0;
               mh_SetWord(acn_Record, mu8_INTEL_ADRH, (u32_Adr & ~mu32_XADR32_MASK));
               mh_SetByte(acn_Record, mu8_INTEL_CMD, mu8_CMD_DATA);
               q_RecordOpen = true;
            }

            // store data
            mh_SetByte(acn_Record, static_cast<uint32_t>(mu8_INTEL_DAT + s32_Length),
                       static_cast<uint8_t>(opu16_BinImage[u32_ByteIndex]));
            s32_Length++;                                                            // inc. data length
            mh_SetByte(acn_Record, mu8_INTEL_LEN, static_cast<uint8_t>(s32_Length)); // set data length

            u32_ByteIndex++;                // increment address index
            if (u32_ByteIndex >= ou32_Size) // end of BinImage?
            {
               break;
            }
         }
         if ((u32_ByteIndex < ou32_Size) && ((opu16_BinImage[u32_ByteIndex] & 0xFF00U) == 0))
         {
            mu32_NumRawBytes--; //no valid data: one byte less in hexfile ...
         }

         if (u32_Error == NO_ERR)
         {
            u32_Error = m_CloseRecord(acn_Record, q_RecordOpen);
         }
         if (u32_Error != NO_ERR)
         {
            break;
         }
      }
   }

   if (u32_Error == NO_ERR)
   {
      m_SetEOFPtr(); // add EOF record to end of file
      u32_Error = m_AddHexLine(":00000001FF\n");
      if (u32_Error == NO_ERR) // add EOF record
      {
         m_RemoveFirst(); // remove first element (zero offset)
      }
   }

   return u32_Error;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Create hex file from data dump

   Uses an array of memory blocks to create a hex file in memory.

   \param[in]  orc_Dump        dump of hexfile
   \param[in]  ou32_RecSize    hex record size 8..255

   \return
   NO_ERR                   no problems
   else                     error
*/
//-----------------------------------------------------------------------------
uint32_t C_HexFile::CreateHexFile(const C_HexDataDump & orc_Dump, const uint32_t ou32_RecSize)
{
   uint32_t u32_Error;
   uint32_t u32_AbsoluteAddress;
   uint32_t u32_RecSize;
   char_t acn_Record[mu32_HEXBUFFER_SIZE];
   int32_t s32_Length = 0;
   int32_t s32_Block;
   uint32_t u32_OffsetInBlock;
   bool q_RecordOpen = false;
   const C_HexDataDumpBlock * pc_Block;

   mq_DumpIsDirty = true;

   u32_RecSize = ou32_RecSize;
   if (u32_RecSize < mu8_MIN_RECSIZE)
   {
      u32_RecSize = mu8_MIN_RECSIZE;
   }
   if (u32_RecSize > mu8_MAX_RECSIZE)
   {
      u32_RecSize = mu8_MAX_RECSIZE;
   }

   this->Clear(); // clear existing hex file data

   u32_Error = m_AddHexLine(":020000040000FA\n"); // init data with zero offset

   if (u32_Error == NO_ERR)
   {
      for (s32_Block = 0U; s32_Block < orc_Dump.at_Blocks.GetLength(); s32_Block++)
      {
         s32_Length = 0;
         pc_Block = &orc_Dump.at_Blocks[s32_Block];

         mu32_NumRawBytes += static_cast<uint32_t>(pc_Block->au8_Data.GetLength());
         for (u32_OffsetInBlock = 0U;
              u32_OffsetInBlock < static_cast<uint32_t>(pc_Block->au8_Data.GetLength());
              u32_OffsetInBlock++)
         {
            u32_AbsoluteAddress = pc_Block->u32_AddressOffset + u32_OffsetInBlock;

            //extended address offset wrap around:
            if ((u32_AbsoluteAddress & mu32_XADR32_MASK) != mu32_AdrOffs)
            {
               u32_Error = m_CloseRecord(acn_Record, q_RecordOpen);
               if (u32_Error == NO_ERR)
               {
                  // create extended 32bit address record
                  mu32_AdrOffs = (u32_AbsoluteAddress & mu32_XADR32_MASK);
                  u32_Error = m_SetOffset(mu32_AdrOffs);
               }
            }

            if (u32_Error == NO_ERR)
            {
               // record size boundary?
               if (((u32_AbsoluteAddress - mu32_AdrOffs) % u32_RecSize) == 0U)
               {
                  u32_Error = m_CloseRecord(acn_Record, q_RecordOpen);
               }
            }

            if (u32_Error != NO_ERR)
            {
               break;
            }

            if (q_RecordOpen == false) // no record open?
            {
               // -> open data record...
               s32_Length = 0;
               mh_SetWord(acn_Record, mu8_INTEL_ADRH, (u32_AbsoluteAddress & ~mu32_XADR32_MASK));
               mh_SetByte(acn_Record, mu8_INTEL_CMD, mu8_CMD_DATA);
               q_RecordOpen = true;
            }

            // store data
            mh_SetByte(acn_Record, static_cast<uint32_t>(mu8_INTEL_DAT + s32_Length),
                       static_cast<uint8_t>(pc_Block->au8_Data[u32_OffsetInBlock]));
            s32_Length++;                                                            // inc. data length
            mh_SetByte(acn_Record, mu8_INTEL_LEN, static_cast<uint8_t>(s32_Length)); // set data length
         }

         if (u32_Error == NO_ERR)
         {
            u32_Error = m_CloseRecord(acn_Record, q_RecordOpen);
         }

         if (u32_Error != NO_ERR)
         {
            break;
         }
      }
   }

   if (u32_Error == 0U)
   {
      m_SetEOFPtr(); // add EOF record to end of file
      u32_Error = m_AddHexLine(":00000001FF\n");
      if (u32_Error == NO_ERR) // add EOF record
      {
         m_RemoveFirst(); // remove first element (zero offset)
      }
   }

   return u32_Error;
}

//************************************************************************
// .FUNCTION    CloseRecord
// .GROUP
//------------------------------------------------------------------------
// .DESCRIPTION
//    if open flag is set: store record data, clear open flag
//------------------------------------------------------------------------
// .PARAMETERS  opcn_Record:         pointer to record data
//              orq_RecordOpen:      flag: record data available
//------------------------------------------------------------------------
// .RETURNVALUE
//              NO_ERR                  0x00000000
//              ERR_NOT_ENOUGH_MEMORY   0xExxxxxxx
//
//              xxxxxxx = line number of optimized file
//************************************************************************
uint32_t C_HexFile::m_CloseRecord(char_t * const opcn_Record, bool & orq_RecordOpen)
{
   uint32_t u32_Error = NO_ERR;

   if (orq_RecordOpen == true) // create valid data record
   {
      mh_CalcCheck(opcn_Record);
      u32_Error = m_AddHexLine(opcn_Record);
      orq_RecordOpen = false;
   }

   return u32_Error;
}

//************************************************************************
// .FUNCTION    HexLineString
// .GROUP
//------------------------------------------------------------------------
// .DESCRIPTION
//  - HexFile.HexLineString converts binary HexLine stored in pbHexLine
//                          into C-string
//------------------------------------------------------------------------
// .PARAMETERS  opu8_HexLine  pointer to binary HexLine
//------------------------------------------------------------------------
// .RETURNVALUE pointer to C-String
//************************************************************************
const char_t * C_HexFile::mh_HexLineString(const uint8_t * const opu8_HexLine) //const
{
   static char_t hacn_Buffer[mu32_HEXBUFFER_SIZE];
   uint32_t u32_Length;

   hacn_Buffer[0] = ':'; // begin a new Intel HEX line
   u32_Length = static_cast<uint32_t>(opu8_HexLine[mu8_INTEL_LEN]) + 5U;

   for (uint32_t u32_ByteIndex = 0U; u32_ByteIndex < u32_Length; u32_ByteIndex++) // store ASCII HEX data
   {
      mh_SetByte(hacn_Buffer, u32_ByteIndex, opu8_HexLine[u32_ByteIndex]);
   }

   u32_Length = (u32_Length * 2U) + 1U; // create line end:
   hacn_Buffer[u32_Length] = '\0';      // terminate string

   return hacn_Buffer;
}

//************************************************************************
// .FUNCTION    GetIHexCommand
// .GROUP
//------------------------------------------------------------------------
// .DESCRIPTION
//    GetIHexCommand    fetch intel command byte from new HEX line
//                      it checks also HEX line syntax
//------------------------------------------------------------------------
// .PARAMETERS  opcn_String   new HEX line (C-string)
//              oru8_Command  reference to command byte
//------------------------------------------------------------------------
// .RETURNVALUE
//              NO_ERR                  0x00000000
//              ERR_HEXLINE_SYNTAX      0x8xxxxxxx
//              ERR_HEXLINE_CHECKSUM    0x9xxxxxxx
//              ERR_HEXLINE_COMMAND     0xAxxxxxxx
//
//              xxxxxxx = line number of input file
//************************************************************************
uint32_t C_HexFile::m_GetIHexCommand(const char_t * const opcn_String, uint8_t & oru8_Command) const
{
   uint32_t u32_Error = ERR_HEXLINE_SYNTAX;
   uint8_t u8_Data;
   uint32_t u32_Loop;
   uint32_t u32_Length;
   uint8_t u8_Checksum = 0U;

   if (opcn_String[0] == ':')
   {
      if (mh_GetByte(opcn_String, mu8_INTEL_LEN, u8_Data) == NO_ERR) // read record data length
      {
         u32_Length = static_cast<uint32_t>(u8_Data) + mu8_INTEL_DAT + 1; // calculate complete record length

         for (u32_Loop = 0U; u32_Loop < u32_Length; u32_Loop++) // calculate checksum...
         {
            u32_Error = mh_GetByte(opcn_String, u32_Loop, u8_Data);
            if (u32_Error != NO_ERR)
            {
               break; // on error break!
            }
            u8_Checksum += u8_Data;
         }

         if ((u32_Error == NO_ERR) && (u8_Checksum != 0x00U)) // checksum error?
         {
            u32_Error = ERR_HEXLINE_CHECKSUM; // yes -> set checksum error
         }
      }

      if (u32_Error == NO_ERR)
      {
         u32_Error = mh_GetByte(opcn_String, mu8_INTEL_CMD, oru8_Command);
         if (u32_Error == NO_ERR)
         {
            if (oru8_Command > mu8_CMD_START32)
            {
               u32_Error = ERR_HEXLINE_COMMAND;
            }
         }
      }
   }

   if (u32_Error != NO_ERR)
   {
      u32_Error |= (mu32_LineCount & ~ERR_MASK); // set line number
   }

   return (u32_Error);
}

//************************************************************************
// .FUNCTION    GetSRecordType
// .GROUP
//------------------------------------------------------------------------
// .DESCRIPTION
//    GetSRecordType fetch s-record type from new HEX line
//------------------------------------------------------------------------
// .PARAMETERS  opcn_String      new HEX line (C-string)
//              oru8_RecordType  reference to srecord type
//------------------------------------------------------------------------
// .RETURNVALUE
//              NO_ERR                  0x00000000
//              ERR_HEXLINE_SYNTAX      0x8xxxxxxx
//              ERR_HEXLINE_CHECKSUM    0x9xxxxxxx
//              ERR_HEXLINE_COMMAND     0xAxxxxxxx
//
//              xxxxxxx = line number of input file
//************************************************************************
uint32_t C_HexFile::m_GetSRecordType(const char_t * const opcn_String, uint8_t & oru8_RecordType) const
{
   uint32_t u32_Error = ERR_HEXLINE_SYNTAX;
   uint8_t u8_Data;
   uint8_t u8_Checksum = 0U;

   if (opcn_String[0] == 'S')
   {
      if (mh_GetByte(opcn_String + 1, mu8_SREC_LEN, u8_Data) == NO_ERR) // read record data length
      {
         int32_t s32_Length = static_cast<int32_t>(u8_Data) + 1; // calculate complete record length

         for (int32_t u32_ByteIndex = 0; u32_ByteIndex < s32_Length; u32_ByteIndex++) // calculate checksum...
         {
            u32_Error = mh_GetByte(opcn_String + 1, u32_ByteIndex, u8_Data);
            if (u32_Error != NO_ERR)
            {
               break;
            }
            u8_Checksum += u8_Data;
         }

         if ((u32_Error == 0U) && (u8_Checksum != 0xFFU)) // checksum error?
         {
            u32_Error = ERR_HEXLINE_CHECKSUM; // yes -> set checksum error
         }
      }

      if (u32_Error == 0U)
      {
         oru8_RecordType = static_cast<uint8_t>(opcn_String[1]) - 0x30U; // read record type

         if (oru8_RecordType > mu8_SREC_END16) // check record type
         {
            u32_Error = ERR_HEXLINE_COMMAND;
         }
      }
   }

   if (u32_Error != NO_ERR)
   {
      u32_Error |= (mu32_LineCount & ~ERR_MASK); // set line number
   }

   return (u32_Error);
}

//************************************************************************
// .FUNCTION    GetIntelAddress
// .GROUP
//------------------------------------------------------------------------
// .DESCRIPTION
//    GetAddress fetch start address from new HEX line
//               it checks also HEX line syntax
//------------------------------------------------------------------------
// .ASSUMPTIONS command byte ou8_Command available
//------------------------------------------------------------------------
// .PARAMETERS  opcn_String   new HEX line (C-string)
//              ou8_Command   Intel HEX command byte
//              oru32_Adr     reference to address
//------------------------------------------------------------------------
// .RETURNVALUE
//              NO_ERR                  0x00000000
//              ERR_HEXLINE_SYNTAX      0x8xxxxxxx
//
//              xxxxxxx = line number of input file
//************************************************************************
uint32_t C_HexFile::m_GetIntelAddress(const char_t * const opcn_String, const uint8_t ou8_Command,
                                      uint32_t & oru32_Adr) const
{
   uint32_t u32_Error = ERR_HEXLINE_SYNTAX;
   uint32_t u32_Address;

   switch (ou8_Command)
   {
   case mu8_CMD_DATA: // calculate 32bit address for data record
      if (mh_GetWord(opcn_String, mu8_INTEL_ADRH, u32_Address) == NO_ERR)
      {
         oru32_Adr = mu32_AdrOffs + u32_Address;
         u32_Error = NO_ERR;
      }
      break;

   case mu8_CMD_XADR16: // calculate 32bit offset address
                        // from Intel16 20bit offset command
      if (mh_GetWord(opcn_String, mu8_INTEL_DAT, u32_Address) == NO_ERR)
      {
         oru32_Adr = (u32_Address << 4);
         u32_Error = NO_ERR;
      }
      break;

   case mu8_CMD_XADR32: // calculate 32bit offset address
      // from Intel32 32bit offset command
      if (mh_GetWord(opcn_String, mu8_INTEL_DAT, u32_Address) == NO_ERR)
      {
         oru32_Adr = (u32_Address << 16U);
         u32_Error = NO_ERR;
      }
      break;

   default:
      break;
   }

   if (u32_Error != NO_ERR)
   {
      u32_Error |= (mu32_LineCount & ~ERR_MASK); // set line number
   }

   return (u32_Error);
}

//************************************************************************
// .FUNCTION    GetSRecordAddress
// .GROUP
//------------------------------------------------------------------------
// .DESCRIPTION
//    GetAddress fetch start address from new HEX line
//               it checks also HEX line syntax
//------------------------------------------------------------------------
// .ASSUMPTIONS command byte available
//------------------------------------------------------------------------
// .PARAMETERS  opcn_String     new HEX line (C-string)
//              ou8_RecordType  S-Record type
//              oru32_Adr       reference to address
//------------------------------------------------------------------------
// .RETURNVALUE
//              NO_ERR                  0x00000000
//              ERR_HEXLINE_SYNTAX      0x8xxxxxxx
//
//              xxxxxxx = line number of input file
//************************************************************************
uint32_t C_HexFile::m_GetSRecordAddress(const char_t * const opcn_String, const uint8_t ou8_RecordType,
                                        uint32_t & oru32_Adr) const
{
   uint32_t u32_Error;
   uint32_t u32_Address;
   uint32_t u32_LSW;
   uint8_t u8_LSB;

   u32_Error = mh_GetWord(opcn_String + 1, mu8_SREC_ADR, u32_Address);
   if (u32_Error == NO_ERR)
   {
      if (ou8_RecordType == mu8_SREC_DATA16)
      {
         oru32_Adr = u32_Address;
      }
      else if (ou8_RecordType == mu8_SREC_DATA24)
      {
         u32_Error = mh_GetByte(opcn_String + 1, mu8_SREC_ADR + 2, u8_LSB);
         if (u32_Error == NO_ERR)
         {
            oru32_Adr = (u32_Address << 8U) | u8_LSB;
         }
      }
      else if (ou8_RecordType == mu8_SREC_DATA32)
      {
         u32_Error = mh_GetWord(opcn_String + 1, mu8_SREC_ADR + 2, u32_LSW);
         if (u32_Error == NO_ERR)
         {
            oru32_Adr = (u32_Address << 16U) | u32_LSW;
         }
      }
      else
      {
         u32_Error = ERR_HEXLINE_SYNTAX;
      }
   }

   if (u32_Error != NO_ERR)
   {
      u32_Error |= (mu32_LineCount & ~ERR_MASK); // set line number
   }

   return (u32_Error);
}

//************************************************************************
// .FUNCTION    SetDataPtr
// .GROUP
//------------------------------------------------------------------------
// .DESCRIPTION
//    SetDataPtr set the internal data pointer to the element with lower
//               or same address as new address (from new data record)
//------------------------------------------------------------------------
// .ASSUMPTIONS at least one element is available
//------------------------------------------------------------------------
// .PARAMETERS  ou32_Adr   new address (from new HEX line)
//------------------------------------------------------------------------
// .RETURNVALUE void
//************************************************************************
void C_HexFile::m_SetDataPtr(const uint32_t ou32_Adr)
{
   T_HexLine * pt_Next;

   if (mpt_Prev == NULL)
   {
      return;
   }

   while (ou32_Adr < mpt_Prev->u32_XAdr) // search for next data entry point
   {
      mpt_Prev = mpt_Prev->pt_Prev;
   }

   pt_Next = mpt_Prev->pt_Next;

   while ((ou32_Adr >= pt_Next->u32_XAdr) && (pt_Next != mpt_DataEntry))
   {
      mpt_Prev = pt_Next;
      pt_Next = mpt_Prev->pt_Next;
   }
}

//************************************************************************
// .FUNCTION    SetXAdrPtr
// .GROUP
//------------------------------------------------------------------------
// .DESCRIPTION
//    SetXAdrPtr set the internal data pointer to the element with lower
//               or same address as new extended address offset
//               if the address offset record already exist return false
//               else return true
//------------------------------------------------------------------------
// .ASSUMPTIONS at least one element is available
//------------------------------------------------------------------------
// .PARAMETERS  ou32_Adr  new address (from new HEX line)
//------------------------------------------------------------------------
// .RETURNVALUE true  address offset record don't exist
//              false address offset record already exist
//************************************************************************
uint32_t C_HexFile::m_SetXAdrPtr(const uint32_t ou32_Adr)
{
   uint32_t u32_Ret = true;
   uint32_t u32_Offs;
   T_HexLine * pt_Next;

   if (mpt_Prev == NULL)
   {
      return false;
   }

   if (ou32_Adr != 0U) // address offset > 0x00000000 ?
   {
      // search entry point for new offset command
      while ((ou32_Adr <= mpt_Prev->u32_XAdr) && (mpt_Prev != mpt_DataEntry))
      {
         mpt_Prev = mpt_Prev->pt_Prev;
      }

      pt_Next = mpt_Prev->pt_Next;
      while ((ou32_Adr > pt_Next->u32_XAdr) && (pt_Next != mpt_DataEntry))
      {
         mpt_Prev = pt_Next;
         pt_Next = mpt_Prev->pt_Next;
      }
      // check if offset command already exists
      if (pt_Next->pu8_HexLine[mu8_INTEL_CMD] == mu8_CMD_XADR32)
      {
         u32_Offs = (static_cast<uint32_t>(pt_Next->pu8_HexLine[mu8_INTEL_DAT]) << 24U) +
                    (static_cast<uint32_t>(pt_Next->pu8_HexLine[mu8_INTEL_DAT + 1U]) << 16U);
         if (u32_Offs == ou32_Adr)
         {
            u32_Ret = false;
         }
      }
   }
   else
   {
      u32_Ret = false; // need no offset for address 0x00000000
   }

   return u32_Ret;
}

//************************************************************************
// .FUNCTION    SetEOFPtr
// .GROUP
//------------------------------------------------------------------------
// .DESCRIPTION
//    SetEOFPtr set the internal data pointer to last element
//------------------------------------------------------------------------
// .ASSUMPTIONS at least one element is available
//------------------------------------------------------------------------
// .PARAMETERS  void
//------------------------------------------------------------------------
// .RETURNVALUE void
//************************************************************************
void C_HexFile::m_SetEOFPtr(void)
{
   mpt_Prev = mpt_DataEntry->pt_Prev; // set ptPrev to last element
}

//************************************************************************
// .FUNCTION    AddHexLine
// .GROUP
//------------------------------------------------------------------------
// .DESCRIPTION
//    AddHexLine insert new HEX line as new data element at current
//               position of internal data pointer
//------------------------------------------------------------------------
// .PARAMETERS  opcn_String  HEX line (C-string)
//------------------------------------------------------------------------
// .RETURNVALUE
//              NO_ERR                  0x00000000
//              ERR_HEXLINE_SYNTAX      0x8xxxxxxx
//              ERR_HEXLINE_CHECKSUM    0x9xxxxxxx
//              ERR_NOT_ENOUGH_MEMORY   0xExxxxxxx
//
//              xxxxxxx = line number of input file
//************************************************************************
uint32_t C_HexFile::m_AddHexLine(const char_t * const opcn_String)
{
   uint32_t u32_Error;
   T_HexLine * pt_New;
   T_HexLine * pt_Next;
   uint8_t u8_Len;
   uint32_t u32_Length;

   u32_Error = mh_GetByte(opcn_String, mu8_INTEL_LEN, u8_Len);
   u32_Length = static_cast<uint32_t>(u8_Len) + 5U;

   if (u32_Error == NO_ERR)
   {
      try // be aware of the bad bad_alloc exception...
      {
         pt_New = new T_HexLine();   // create new element
         pt_New->pu8_HexLine = NULL; // sorry, no data!

         if (mpt_Prev == NULL) // first entry?
         {
            pt_New->pt_Prev = pt_New; // 1st element points to himself
            pt_New->pt_Next = pt_New; // 1st element points to himself
            mpt_Prev      = pt_New;   // new element is also previous element!
            mpt_DataEntry = pt_New;   // remember the entry pointer!
         }

         pt_Next = mpt_Prev->pt_Next; // remember next element!
         mpt_Prev->pt_Next = pt_New;  // previous points to new element
         pt_New->pt_Prev = mpt_Prev;  // new points to previous element
         pt_New->pt_Next = pt_Next;   // new points also to next element
         pt_Next->pt_Prev = pt_New;   // next points to new element
         mpt_Prev = pt_New;           // next time new is previous...

         pt_New->pu8_HexLine = new uint8_t[u32_Length]; // allocate space for data
      }
      catch (...) // UWE! need some RAM
      {
         u32_Error = ERR_NOT_ENOUGH_MEMORY;
      }

      if (u32_Error == NO_ERR)
      {
         u32_Error = m_CopyData(opcn_String, pt_New, u32_Length);
      }
   }

   if (u32_Error != NO_ERR)
   {
      u32_Error |= (mu32_LineCount & ~ERR_MASK); // set line number info
   }

   return (u32_Error);
}

//************************************************************************
// .FUNCTION    CopyData
// .GROUP
//------------------------------------------------------------------------
// .DESCRIPTION
//    CopyData copy data from new HEX line into new data element
//             it also checks HEX line syntax and calculates
//             start address of HEX record, min/max address
//------------------------------------------------------------------------
// .PARAMETERS  opcn_String   HEX line (C-string)
//              opt_HexLine   pointer to new HEX line element
//              ou32_Length   record length
//------------------------------------------------------------------------
// .RETURNVALUE
//              NO_ERR                  0x00000000
//              ERR_HEXLINE_SYNTAX      0x8xxxxxxx
//              ERR_HEXLINE_CHECKSUM    0x9xxxxxxx
//
//              xxxxxxx = line number of input file
//************************************************************************
uint32_t C_HexFile::m_CopyData(const char_t * const opcn_String, T_HexLine * const opt_HexLine,
                               const uint32_t ou32_Length)
{
   uint32_t u32_Error = NO_ERR;
   uint32_t u32_StartAdr;
   uint32_t u32_EndAdr;
   uint8_t u8_Data;

   for (uint32_t u32_ByteIndex = 0U; u32_ByteIndex < ou32_Length; u32_ByteIndex++) // copy HEX data into buffer
   {
      u32_Error = mh_GetByte(opcn_String, u32_ByteIndex, u8_Data);
      if (u32_Error != 0U)
      {
         break;
      }

      opt_HexLine->pu8_HexLine[u32_ByteIndex] = u8_Data;
   }

   if (u32_Error == 0U) // no error in HEX data ?
   {
      // 32bit address offset command ?
      // calculate and store 32bit start address
      if (opt_HexLine->pu8_HexLine[mu8_INTEL_CMD] == mu8_CMD_XADR32)
      {
         opt_HexLine->u32_XAdr = ((static_cast<uint32_t>(opt_HexLine->pu8_HexLine[mu8_INTEL_DAT]) << 24U) +
                                  (static_cast<uint32_t>(opt_HexLine->pu8_HexLine[mu8_INTEL_DAT + 1]) << 16U));
      }

      // data record ?
      if (opt_HexLine->pu8_HexLine[mu8_INTEL_CMD] == mu8_CMD_DATA)
      {
         // calculate start address
         u32_StartAdr = ((static_cast<uint32_t>(opt_HexLine->pu8_HexLine[mu8_INTEL_ADRH]) << 8U) +
                         opt_HexLine->pu8_HexLine[mu8_INTEL_ADRL]) + mu32_AdrOffs;
         u32_EndAdr = (u32_StartAdr + ou32_Length) - 6;
         if (u32_StartAdr < mu32_MinAdr) // check and store min address
         {
            mu32_MinAdr = u32_StartAdr;
         }
         if (u32_EndAdr > mu32_MaxAdr) // check and store max address
         {
            mu32_MaxAdr = u32_EndAdr;
         }

         opt_HexLine->u32_XAdr = u32_StartAdr; // store 32bit start address
      }
   }

   if (u32_Error != NO_ERR)
   {
      u32_Error |= (mu32_LineCount & ~ERR_MASK); // set line number info
   }
   else
   {
      mu32_LineCountNew++; // count new line numbers
   }

   return u32_Error;
}

//************************************************************************
// .FUNCTION    RemoveFirst
// .GROUP
//------------------------------------------------------------------------
// .DESCRIPTION
//    RemoveFirst removes first HEX record entry
//                (offset command to address 0x00000000)
//------------------------------------------------------------------------
// .PARAMETERS  void
//------------------------------------------------------------------------
// .RETURNVALUE void
//************************************************************************
void C_HexFile::m_RemoveFirst(void)
{
   T_HexLine * pt_Next;
   T_HexLine * pt_Last;

   if (mpt_DataEntry != NULL)
   {
      pt_Next = mpt_DataEntry->pt_Next; // pointer to next element
      pt_Last = mpt_DataEntry->pt_Prev; // pointer to last element

      pt_Next->pt_Prev = pt_Last; // previous from next is last element
      pt_Last->pt_Next = pt_Next; // next from previos is next (now 1st element)

      if (mpt_Prev == mpt_DataEntry) // check and change global pointers
      {
         mpt_Prev = pt_Next;
      }
      if (mpt_HexData == mpt_DataEntry)
      {
         mpt_HexData = pt_Next;
      }
      // remove data buffer
      if (mpt_DataEntry->pu8_HexLine != NULL)
      {
         delete[] mpt_DataEntry->pu8_HexLine;
      }

      delete mpt_DataEntry; // remove element

      mpt_DataEntry = pt_Next; // next is 1st element

      mu32_LineCountNew--; // dec line counter
   }
}

//************************************************************************
// .FUNCTION    ConvOffs16To32
// .GROUP
//------------------------------------------------------------------------
// .DESCRIPTION
//    ConvOffs16To32 convert a Intel16 offset command to Intel32 offset
//------------------------------------------------------------------------
// .PARAMETERS  opcn_String  HEX line (C-string)
//------------------------------------------------------------------------
// .RETURNVALUE
//              NO_ERR                  0x00000000
//              ERR_HEXLINE_SYNTAX      0x8xxxxxxx
//
//              xxxxxxx = line number of input file
//************************************************************************
uint32_t C_HexFile::m_ConvOffs16To32(char_t * const opcn_String) const
{
   uint32_t u32_Error;
   uint32_t u32_AdrOffs;

   u32_Error = mh_GetWord(opcn_String, mu8_INTEL_DAT, u32_AdrOffs);
   if (u32_Error == NO_ERR)
   {
      u32_AdrOffs >>= 12;
      mh_SetWord(opcn_String, mu8_INTEL_DAT, u32_AdrOffs);
      mh_SetByte(opcn_String, mu8_INTEL_CMD, mu8_CMD_XADR32);

      u32_Error = mh_CalcCheck(opcn_String);
   }

   if (u32_Error != NO_ERR)
   {
      u32_Error |= (mu32_LineCount & ~ERR_MASK); // set line number info
   }

   return (u32_Error);
}

//************************************************************************
// .FUNCTION    ConvRec16ToRec32
// .GROUP
//------------------------------------------------------------------------
// .DESCRIPTION
//    ConvOffs16To32 convert Intel16 data records to Intel32 data records
//           creates new offset commands and splits data records that
//            do not fit into 64K pages
//           all HEX data will be stored in memory
//------------------------------------------------------------------------
// .PARAMETERS  opcn_String  HEX line (C-string)
//------------------------------------------------------------------------
// .RETURNVALUE
//              NO_ERR                  0x00000000
//              ERR_HEXLINE_SYNTAX      0x8xxxxxxx
//              ERR_HEXLINE_CHECKSUM    0x9xxxxxxx
//              ERR_NOT_ENOUGH_MEMORY   0xExxxxxxx
//
//              xxxxxxx = line number of input file
//************************************************************************
uint32_t C_HexFile::m_ConvRec16ToRec32(char_t * const opcn_String)
{
   uint32_t u32_Error;
   char_t acn_HexLine[mu32_HEXBUFFER_SIZE];
   uint32_t u32_Adr;
   uint8_t u8_Len;
   uint8_t u8_Len1;
   uint32_t u32_StartAdr;
   uint32_t u32_EndAdr;
   uint32_t u32_SaveOffs;

   u32_Error = mh_GetWord(opcn_String, mu8_INTEL_ADRH, u32_Adr);
   if (u32_Error == NO_ERR)
   {
      u32_Error = mh_GetByte(opcn_String, mu8_INTEL_LEN, u8_Len);
      if (u32_Error == NO_ERR)
      {
         u32_StartAdr = (mu32_AdrOffs & ~mu32_XADR32_MASK) + u32_Adr;
         u32_EndAdr   = (u32_StartAdr + u8_Len) - 1;

         if (u32_StartAdr >= 0x00010000U)
         {
            // write 32bit offset command
            u32_Error = m_SetOffset((mu32_AdrOffs & mu32_XADR32_MASK) + 0x10000U);
         }

         m_SetDataPtr(mu32_AdrOffs + u32_Adr); // calc new list pointer

         if (u32_Error == NO_ERR)
         {
            if ((u32_EndAdr < 0x00010000U) ||
                ((u32_StartAdr >= 0x00010000U) && (u32_EndAdr < 0x00020000U)))
            {
               mh_SetWord(opcn_String, mu8_INTEL_ADRH, u32_StartAdr);
               mh_CalcCheck(opcn_String);

               u32_SaveOffs = mu32_AdrOffs;                                // save address offset
               mu32_AdrOffs = (mu32_AdrOffs + u32_Adr) & mu32_XADR32_MASK; // create 32bit Offset
               u32_Error = m_AddHexLine(opcn_String);                      // add 32bit HEX record
               mu32_AdrOffs = u32_SaveOffs;                                // restore address offset
            }
            else
            {
               u8_Len1 = static_cast<uint8_t>(0x00010000U - (u32_StartAdr & ~mu32_XADR32_MASK));
               u8_Len  -= u8_Len1;
               // create 1st part of 32bit record:
               acn_HexLine[0] = ':';
               mh_SetByte(acn_HexLine, mu8_INTEL_LEN, u8_Len1);
               mh_SetWord(acn_HexLine, mu8_INTEL_ADRH, u32_StartAdr);
               mh_SetByte(acn_HexLine, mu8_INTEL_CMD, mu8_CMD_DATA);
               for (uint8_t u32_ByteIndex = 0U; u32_ByteIndex < u8_Len1; u32_ByteIndex++)
               {
                  strncpy(&acn_HexLine[(u32_ByteIndex * 2U) + 9U], &opcn_String[(u32_ByteIndex * 2U) + 9U], 2U);
               }
               mh_CalcCheck(acn_HexLine);
               strncpy(&acn_HexLine[(static_cast<uint32_t>(u8_Len1) * 2U) + 11U], "\n\0", 2U);

               u32_SaveOffs = mu32_AdrOffs;                                // save address offset
               mu32_AdrOffs = (mu32_AdrOffs + u32_Adr) & mu32_XADR32_MASK; // create 32bit Offset
               u32_Error = m_AddHexLine(acn_HexLine);                      // add 32bit HEX record
               mu32_AdrOffs = u32_SaveOffs;                                // restore address offset

               if (u32_Error == NO_ERR)
               {
                  // write 32bit offset command
                  u32_StartAdr = (((mu32_AdrOffs + u32_Adr) & mu32_XADR32_MASK) + 0x10000U);
                  u32_Error = m_SetOffset(u32_StartAdr);
                  if (u32_Error == NO_ERR)
                  {
                     // create 2nd part of 32bit record:
                     mh_SetByte(acn_HexLine, mu8_INTEL_LEN, u8_Len);
                     mh_SetWord(acn_HexLine, mu8_INTEL_ADRH, u32_StartAdr);
                     for (uint32_t i = u8_Len1; i < (static_cast<uint32_t>(u8_Len) + u8_Len1); i++)
                     {
                        strncpy(&acn_HexLine[((i - u8_Len1) * 2U) + 9U], &opcn_String[(i * 2U) + 9U], 2U);
                     }
                     mh_CalcCheck(acn_HexLine);
                     strncpy(&acn_HexLine[(static_cast<uint32_t>(u8_Len) * 2U) + 11U], "\n\0", 2U);

                     m_SetDataPtr(u32_StartAdr);            // calc new list pointer
                     u32_SaveOffs = mu32_AdrOffs;           // save address offset
                     mu32_AdrOffs = u32_StartAdr;           // create 32bit Offset
                     u32_Error = m_AddHexLine(acn_HexLine); // add 32bit HEX record
                     mu32_AdrOffs = u32_SaveOffs;           // restore address offset
                  }
               }
            }
         }
      }
   }

   if (u32_Error != NO_ERR)
   {
      u32_Error |= (mu32_LineCount & ~ERR_MASK); // set line number info
   }
   return (u32_Error);
}

//************************************************************************
// .FUNCTION    SetOffset
// .GROUP
//------------------------------------------------------------------------
// .DESCRIPTION
//    SetOffset insert a Intel32 offset command at address dwAdr
//              do nothing if offset command already exist
//------------------------------------------------------------------------
// .PARAMETERS  ou32_Adr  32bit offset address
//------------------------------------------------------------------------
// .RETURNVALUE
//              NO_ERR                  0x00000000
//              ERR_HEXLINE_SYNTAX      0x8xxxxxxx
//              ERR_HEXLINE_CHECKSUM    0x9xxxxxxx
//              ERR_NOT_ENOUGH_MEMORY   0xExxxxxxx
//
//              xxxxxxx = line number of input file
//************************************************************************
uint32_t C_HexFile::m_SetOffset(const uint32_t ou32_Adr)
{
   uint32_t u32_Error = NO_ERR;
   char_t acn_Offset[] = ":02000004000000\n";

   if (m_SetXAdrPtr(ou32_Adr) != 0)
   {
      mh_SetWord(acn_Offset, mu8_INTEL_DAT, (ou32_Adr >> 16U));
      mh_CalcCheck(acn_Offset);
      u32_Error = m_AddHexLine(acn_Offset);
   }

   return u32_Error;
}

//************************************************************************
// .FUNCTION    CalcCheck
// .GROUP
//------------------------------------------------------------------------
// .DESCRIPTION
//    CalcCheck calculate and insert checksum into HEX line
//------------------------------------------------------------------------
// .PARAMETERS  opcn_String   HEX line (C-string)
//------------------------------------------------------------------------
// .RETURNVALUE
//              NO_ERR                  0x00000000
//              ERR_HEXLINE_SYNTAX      0x80000000
//************************************************************************
uint32_t C_HexFile::mh_CalcCheck(char_t * const opcn_String)
{
   uint32_t u32_Error;
   uint32_t u32_Length;
   uint8_t u8_Data;
   uint8_t u8_Checksum = 0U;

   u32_Error = mh_GetByte(opcn_String, mu8_INTEL_LEN, u8_Data);
   if (u32_Error == NO_ERR)
   {
      u32_Length = static_cast<uint32_t>(u8_Data) + 4U;

      for (uint32_t i = 0U; i < u32_Length; i++)
      {
         u32_Error = mh_GetByte(opcn_String, i, u8_Data);
         if (u32_Error != NO_ERR)
         {
            break;
         }
         u8_Checksum += u8_Data;
      }

      if (u32_Error == NO_ERR)
      {
         u8_Checksum = static_cast<uint8_t>((~u8_Checksum) + 1U);
         mh_SetByte(opcn_String, u32_Length, u8_Checksum);
      }
   }

   return u32_Error;
}

//************************************************************************
// .FUNCTION    GetByte
// .GROUP
//------------------------------------------------------------------------
// .DESCRIPTION
//    GetByte read byte out of HEX line at position dwIndex
//------------------------------------------------------------------------
// .PARAMETERS  opcn_String    HEX line (C-string)
//              ou32_Index     position inside HEX record
//              oru8_Data      reference to data byte
//------------------------------------------------------------------------
// .RETURNVALUE
//              NO_ERR                  0x00000000
//              ERR_HEXLINE_SYNTAX      0x80000000
//************************************************************************
uint32_t C_HexFile::mh_GetByte(const char_t * const opcn_String, const uint32_t ou32_Index, uint8_t & oru8_Data)
{
   uint32_t u32_Error = ERR_HEXLINE_SYNTAX;
   uint32_t u32_Index = ou32_Index;
   uint8_t u8_HiNib;
   uint8_t u8_LoNib;

   u32_Index = (u32_Index * 2U) + 1U;
   u8_HiNib = static_cast<uint8_t>(opcn_String[u32_Index]);
   u8_LoNib = static_cast<uint8_t>(opcn_String[u32_Index + 1]);

   if ((u8_HiNib >= static_cast<uint8_t>('0')) && (u8_HiNib <= static_cast<uint8_t>('9')))
   {
      u8_HiNib = static_cast<uint8_t>((static_cast<uint8_t>(u8_HiNib - 0x30U)) << 4);
   }
   else
   {
      u8_HiNib |= 0x20U; // A..F to lower case a..f
      if ((u8_HiNib >= 0x61U) && (u8_HiNib <= 0x66U))
      {
         u8_HiNib = static_cast<uint8_t>((static_cast<uint8_t>(u8_HiNib - 0x57U)) << 4);
      }
      else
      {
         return (u32_Error);
      }
   }

   if ((u8_LoNib >= static_cast<uint8_t>('0')) && (u8_LoNib <= static_cast<uint8_t>('9')))
   {
      u8_LoNib = static_cast<uint8_t>(u8_LoNib - 0x30U);
   }
   else
   {
      u8_LoNib |= 0x20U; // A..F to lower case a..f
      if ((u8_LoNib >= 0x61U) && (u8_LoNib <= 0x66U))
      {
         u8_LoNib = static_cast<uint8_t>(u8_LoNib - 0x57U);
      }
      else
      {
         return (u32_Error);
      }
   }

   oru8_Data = (u8_HiNib | u8_LoNib);
   return NO_ERR;
}

//************************************************************************
// .FUNCTION    SetByte
// .GROUP
//------------------------------------------------------------------------
// .DESCRIPTION
//    SetByte write one byte into HEX line at position dwIndex
//------------------------------------------------------------------------
// .PARAMETERS  opcn_String   HEX line (C-string)
//              ou32_Index    position inside HEX record
//              ou8_Data      data to write
//------------------------------------------------------------------------
// .RETURNVALUE void
//************************************************************************
void C_HexFile::mh_SetByte(char_t * const opcn_String, const uint32_t ou32_Index, const uint8_t ou8_Byte)
{
   uint8_t au8_HexByte[2];
   uint32_t u32_Index;

   au8_HexByte[0] = static_cast<uint8_t>(ou8_Byte >> 4);
   au8_HexByte[0] = static_cast<uint8_t>(au8_HexByte[0] + 0x30U); // create HEX ASCII char...
   if (au8_HexByte[0] > 0x39U)
   {
      au8_HexByte[0] = static_cast<uint8_t>(au8_HexByte[0] + 7U);
   }

   au8_HexByte[1] = static_cast<uint8_t>(ou8_Byte & 0x0FU);
   au8_HexByte[1] = static_cast<uint8_t>(au8_HexByte[1] + 0x30U); // create HEX ASCII char...
   if (au8_HexByte[1] > 0x39U)
   {
      au8_HexByte[1] = static_cast<uint8_t>(au8_HexByte[1] + 7U);
   }

   u32_Index = (ou32_Index * 2U) + 1U;
   opcn_String[u32_Index]     = static_cast<char_t>(au8_HexByte[0]);
   opcn_String[u32_Index + 1] = static_cast<char_t>(au8_HexByte[1]);
}

//************************************************************************
// .FUNCTION    GetWord
// .GROUP
//------------------------------------------------------------------------
// .DESCRIPTION
//    GetWord read word out of HEX line at position dwIndex
//------------------------------------------------------------------------
// .PARAMETERS  opcn_String   HEX line (C-string)
//              ou32_Index    position inside HEX record
//              oru32_Word    reference to data word
//------------------------------------------------------------------------
// .RETURNVALUE
//              NO_ERR                  0x00000000
//              ERR_HEXLINE_SYNTAX      0x80000000
//************************************************************************
uint32_t C_HexFile::mh_GetWord(const char_t * const opcn_String, const uint32_t ou32_Index, uint32_t & oru32_Word)
{
   uint32_t u32_Error;
   uint8_t u8_Hi;
   uint8_t u8_Lo;

   u32_Error = mh_GetByte(opcn_String, ou32_Index, u8_Hi);
   if (u32_Error == NO_ERR)
   {
      u32_Error = mh_GetByte(opcn_String, ou32_Index + 1, u8_Lo);
      if (u32_Error == NO_ERR)
      {
         oru32_Word = (static_cast<uint32_t>(u8_Hi) << 8U) + u8_Lo;
      }
   }

   return u32_Error;
}

//************************************************************************
// .FUNCTION    SetWord
// .GROUP
//------------------------------------------------------------------------
// .DESCRIPTION
//    SetWord write one word into HEX line at position ou32_Index
//------------------------------------------------------------------------
// .PARAMETERS  opcn_String  HEX line (C-string)
//              ou32_Index   position inside HEX record
//              ou32_Data    data to write
//------------------------------------------------------------------------
// .RETURNVALUE void
//************************************************************************
void C_HexFile::mh_SetWord(char_t * const opcn_String, const uint32_t ou32_Index, const uint32_t ou32_Data)
{
   mh_SetByte(opcn_String, ou32_Index,     static_cast<uint8_t>(ou32_Data >> 8U));
   mh_SetByte(opcn_String, ou32_Index + 1, static_cast<uint8_t>(ou32_Data));
}

//------------------------------------------------------------------------

//check whether data from hexfile is valid
//- address used twice
uint32_t C_HexFile::Validate(void)
{
   uint32_t u32_Error;

   (void)this->GetDataDump(u32_Error);
   return u32_Error;
}

//------------------------------------------------------------------------

uint32_t C_HexFile::GetXAdrActLine(uint32_t & oru32_XAdr) const
{
   uint32_t u32_Return = ERR_NO_DATA;

   if (mpt_HexData != NULL)
   {
      oru32_XAdr = mpt_HexData->u32_XAdr;
      u32_Return = NO_ERR;
   }
   return u32_Return;
}

//------------------------------------------------------------------------

uint32_t C_HexFile::LineCount(void) const
{
   return (mu32_LineCountNew);
}

//------------------------------------------------------------------------

uint32_t C_HexFile::MinAdr(void) const
{
   return (mu32_MinAdr);
}

//------------------------------------------------------------------------

uint32_t C_HexFile::MaxAdr(void) const
{
   return (mu32_MaxAdr);
}

//------------------------------------------------------------------------

const uint8_t * C_HexFile::LineInit(void)
{
   uint8_t * pu8_Start = NULL;

   mpt_HexData = mpt_DataEntry;
   if (mpt_HexData != NULL)
   {
      pu8_Start = mpt_HexData->pu8_HexLine;
   }
   return pu8_Start;
}

//------------------------------------------------------------------------

const uint8_t * C_HexFile::NextLine(void)
{
   uint8_t * pu8_Line = NULL;

   if (mpt_HexData != NULL)
   {
      pu8_Line = mpt_HexData->pu8_HexLine;
      if (mpt_HexData->pt_Next != mpt_DataEntry)
      {
         mpt_HexData = mpt_HexData->pt_Next;
      }
      else
      {
         mpt_HexData = NULL;
      }
   }
   return (pu8_Line);
}

//-----------------------------------------------------------------------------
/*!
   \brief   returns a number of bytes from an absolute address

   Returns an array of bytes at the specified address.

   \param[in]     ou32_Address   absolute address to read the data from
   \param[in,out] oru16_NumBytes in:  number of bytes to read
                                 out: number of bytes read
   \param[out]    opu8_Data      data at the specified address

   \return
   0        data read (result in opu8_Data)
   -1       error (data not found at specified address)
   -2       data read (result in opu8_Data), but not all bytes were available before the next memory gap after
             ou32_Address
*/
//-----------------------------------------------------------------------------
int32_t C_HexFile::GetDataByAddress(const uint32_t ou32_Address, uint16_t & oru16_NumBytes, uint8_t * const opu8_Data)
{
   int32_t s32_Block;
   uint32_t u32_Offset;
   uint32_t u32_BlockLength;
   const C_HexDataDump * pc_HexFileData;
   int32_t s32_Return;
   uint16_t u16_NumBytes;
   uint32_t u32_Return;

   pc_HexFileData = this->GetDataDump(u32_Return);
   if (u32_Return != NO_ERR)
   {
      return -1;
   }

   for (s32_Block = 0; s32_Block < pc_HexFileData->at_Blocks.GetLength(); s32_Block++)
   {
      u32_BlockLength = pc_HexFileData->at_Blocks[s32_Block].au8_Data.GetLength();
      if ((ou32_Address <= (pc_HexFileData->at_Blocks[s32_Block].u32_AddressOffset + u32_BlockLength)) &&
          (ou32_Address >= pc_HexFileData->at_Blocks[s32_Block].u32_AddressOffset))
      {
         //it's in this block !
         //it must be completely in this block, as we do not want to read undefined data from any gaps !
         u32_Offset = ou32_Address - pc_HexFileData->at_Blocks[s32_Block].u32_AddressOffset; //offset within the block

         if ((u32_BlockLength - u32_Offset) < static_cast<uint32_t>(oru16_NumBytes))
         {
            s32_Return = -2; //not enough bytes !
            u16_NumBytes = static_cast<uint16_t>(u32_BlockLength - u32_Offset);
            oru16_NumBytes = u16_NumBytes;
         }
         else
         {
            s32_Return = 0; //not enough bytes !
            u16_NumBytes = oru16_NumBytes;
         }
         (void)std::memcpy(opu8_Data, &pc_HexFileData->at_Blocks[s32_Block].au8_Data[u32_Offset], u16_NumBytes);
         return s32_Return;
      }
   }
   return -1;
}

//-----------------------------------------------------------------------------
/*!
   \brief   find a pattern in memory

   Finds a specified data pattern in memory and returns the offset in the searched buffer.

   \param[in]     opu8_Buffer        buffer to search
   \param[in]     opu8_Pattern       pattern to find
   \param[in]     ou32_BufSize       size of opu8_Buffer
   \param[in]     ou16_PatternLength length of pattern to match

   \return
   >= 0     pattern found (index within opu8_Buffer)
   -1       error (pattern not found)
*/
//-----------------------------------------------------------------------------
int32_t C_HexFile::mh_FindPattern(const uint8_t * const opu8_Buffer, const uint8_t * const opu8_Pattern,
                                  const uint32_t ou32_BufSize, const uint16_t ou16_PatternLength)
{
   uint32_t u32_Index;
   uint32_t u32_LastPossiblePosition;
   int32_t s32_Return;

   if (ou32_BufSize > 0x7FFFFFFFUL) //as we return an sint32 we can't handle block bigger than 2GB
   {
      return -1;
   }

   if (ou32_BufSize < static_cast<uint32_t>(ou16_PatternLength))
   {
      return -1;
   }

   u32_LastPossiblePosition = ou32_BufSize - ou16_PatternLength;

   for (u32_Index = 0U; u32_Index <= u32_LastPossiblePosition; u32_Index++)
   {
      if (opu8_Buffer[u32_Index] == opu8_Pattern[0])
      {
         //check rest:
         s32_Return = std::memcmp(&opu8_Buffer[u32_Index + 1U], &opu8_Pattern[1],
                                  ou16_PatternLength - 1);
         if (s32_Return == 0)
         {
            return static_cast<int32_t>(u32_Index);
         }
      }
   }
   return -1;
}

//-----------------------------------------------------------------------------
/*!
   \brief   find a memory pattern

   Finds a specified data pattern in memory and returns the address offset.

   \param[in,out] oru32_Address      in:  start address to search from
                                     out: address of pattern
   \param[in]     ou8_PatternLength  length of the pattern in bytes
   \param[in]     opu8_Pattern       pattern to find

   \return
   0        pattern found (address in opu32_Address)
   -1       error (pattern not found)
*/
//-----------------------------------------------------------------------------
int32_t C_HexFile::FindPattern(uint32_t & oru32_Address, const uint8_t ou8_PatternLength,
                               const uint8_t * const opu8_Pattern)
{
   int32_t s32_Return;
   int32_t s32_Block;
   uint32_t u32_Offset;
   uint32_t u32_BlockLength;
   uint32_t u32_Return;

   const C_HexDataDump * pc_HexFileData;

   pc_HexFileData = this->GetDataDump(u32_Return);
   if (u32_Return != NO_ERR)
   {
      return -1;
   }

   for (s32_Block = 0; s32_Block < pc_HexFileData->at_Blocks.GetLength(); s32_Block++)
   {
      u32_BlockLength = pc_HexFileData->at_Blocks[s32_Block].au8_Data.GetLength();
      if (oru32_Address < (pc_HexFileData->at_Blocks[s32_Block].u32_AddressOffset + u32_BlockLength))
      {
         //is the start address exactly in this block ?
         if (oru32_Address >= pc_HexFileData->at_Blocks[s32_Block].u32_AddressOffset)
         {
            //start address is in this block !
            //set offset within the block
            u32_Offset = oru32_Address - pc_HexFileData->at_Blocks[s32_Block].u32_AddressOffset;
         }
         else
         {
            //start at the beginning of this block
            u32_Offset = 0U;
         }

         s32_Return = mh_FindPattern(&pc_HexFileData->at_Blocks[s32_Block].au8_Data[u32_Offset], opu8_Pattern,
                                     u32_BlockLength - u32_Offset, ou8_PatternLength);
         if (s32_Return >= 0) //found in this block !
         {
            oru32_Address = (pc_HexFileData->at_Blocks[s32_Block].u32_AddressOffset + u32_Offset +
                             static_cast<uint32_t>(s32_Return));
            return 0;
         }
      }
   }
   return -1;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Extract hex file data to structure

   As hex-files can have gaps we can not simply create a binary image.
   - the gaps might be too huge, so that we'd consume huge amounts of RAM
   - we'd need an additional flag for each byte to show whether it is occupied
   So we use a dynamic structure holding all the individual "blocks" of the hex-file.
   As soon as there is a gap of at least one byte between a byte and the next we
    interpret this as a new block.

   Will also check for addresses in hex file occupied more than once.

   Assumption: hex file data is sorted linear in instance data (done by LoadFromFile())

   \param[out]  oru32_ErrorResult   error code:
        NO_ERR                  data dump created
        WRN_RECORD_OVERLAY      address in hex file multiply used (lowest 7 nibbles contain lowest 7 nibbles of the offending address)
        ERR_NOT_ENOUGH_MEMORY   out of memory
        ERR_NO_DATA             no data available

   \return
   address  OK (address of data)
   NULL     error
*/
//-----------------------------------------------------------------------------
const C_HexDataDump * C_HexFile::GetDataDump(uint32_t & oru32_ErrorResult)
{
   uint32_t u32_NumBlocks;
   int64_t s64_LastAddress;
   int32_t s32_BlockIndex;
   uint32_t u32_BlockSize;
   uint32_t u32_Index;
   const uint8_t * pu8_Data;
   uint32_t u32_Address;
   uint8_t u8_Size;
   bool q_FirstBlock;

   if (mq_DumpIsDirty == false)
   {
      oru32_ErrorResult = NO_ERR;
      return &this->mc_Dump;
   }
   //First go through whole data and detect number of blocks
   //This will improve speed as we do not need to resize the DynamicArray, potentially
   // already containing a lot of data, all the time.
   pu8_Data = LineInit();
   if (pu8_Data == NULL)
   {
      oru32_ErrorResult = ERR_NO_DATA;
      return NULL;
   }

   s64_LastAddress = -2; //-2 -> make sure that a block is detected at the first line
   u32_NumBlocks = 0U;
   while (pu8_Data != NULL)
   {
      pu8_Data = NextBinData(u32_Address, u8_Size);
      if (pu8_Data != NULL)
      {
         //memory overlap ? (we assume hex file data already sorted by address)
         if (static_cast<int64_t>(u32_Address) < s64_LastAddress)
         {
            oru32_ErrorResult = WRN_RECORD_OVERLAY | ((u32_Address) & ~ERR_MASK);
            mu32_LastOverlayErrorAddress = u32_Address;
            return NULL;
         }
         else if (static_cast<int64_t>(u32_Address) > s64_LastAddress)
         {
            u32_NumBlocks++; //new block detected
         }
         else
         {
            //nothing to do ...
         }

         s64_LastAddress = static_cast<int64_t>(u32_Address) + u8_Size;
      }
   }

   //now size the array accordingly:
   try
   {
      mc_Dump.at_Blocks.SetLength(u32_NumBlocks);
   }
   catch (...)
   {
      oru32_ErrorResult = ERR_NOT_ENOUGH_MEMORY;
      return NULL;
   }

   pu8_Data = LineInit();
   s64_LastAddress = -2; //-2 -> make sure that a block is detected at the first line
   s32_BlockIndex = 0;
   u32_BlockSize = 0U;

   //again: first check how big each block is for performance reasons ...
   q_FirstBlock = true;
   while (pu8_Data != NULL)
   {
      pu8_Data = NextBinData(u32_Address, u8_Size);
      if (pu8_Data != NULL)
      {
         if (q_FirstBlock == true) //start of block
         {
            //set offset of block:
            mc_Dump.at_Blocks[s32_BlockIndex].u32_AddressOffset = u32_Address;
            q_FirstBlock = false;
         }

         if ((static_cast<int64_t>(u32_Address) > s64_LastAddress) && (u32_BlockSize > 0U))
         {
            //new block -> set size of last one
            try
            {
               mc_Dump.at_Blocks[s32_BlockIndex].au8_Data.SetLength(u32_BlockSize);
            }
            catch (...)
            {
               oru32_ErrorResult = ERR_NOT_ENOUGH_MEMORY;
               return NULL;
            }
            s32_BlockIndex++; //new block detected
            u32_BlockSize = 0U;
            mc_Dump.at_Blocks[s32_BlockIndex].u32_AddressOffset = u32_Address;
         }
         u32_BlockSize += u8_Size;
         s64_LastAddress = static_cast<int64_t>(u32_Address) + u8_Size;
      }
   }
   //final block:
   try
   {
      mc_Dump.at_Blocks[s32_BlockIndex].au8_Data.SetLength(u32_BlockSize);
   }
   catch (...)
   {
      oru32_ErrorResult = ERR_NOT_ENOUGH_MEMORY;
      return NULL;
   }

   //finally: get the data:
   s64_LastAddress = -2; //-2 -> make sure that a block is detected at the first line
   s32_BlockIndex = 0;
   u32_Index = 0U;

   pu8_Data = LineInit();
   while (pu8_Data != NULL)
   {
      pu8_Data = NextBinData(u32_Address, u8_Size);
      if (pu8_Data != NULL)
      {
         if ((static_cast<int64_t>(u32_Address) > s64_LastAddress) && (u32_Index > 0U))
         {
            s32_BlockIndex++; //new block detected
            u32_Index = 0U;
         }
         (void)std::memcpy(&mc_Dump.at_Blocks[s32_BlockIndex].au8_Data[u32_Index], pu8_Data, u8_Size);

         u32_Index += u8_Size;
         s64_LastAddress = static_cast<int64_t>(u32_Address) + u8_Size;
      }
   }
   oru32_ErrorResult = NO_ERR;
   mq_DumpIsDirty = false;
   return &this->mc_Dump;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Return address of last memory overlay error

   If this class returns WRN_RECORD_OVERLAY from any function only the lowest 7 nibbles contain the
   offending address.
   Use this function to read the address including the most significant nibble.

   \return
   Offending address when generating last WRN_RECORD_OVERLAY return code.
*/
//-----------------------------------------------------------------------------
uint32_t C_HexFile::GetLastOverlayErrorAddress(void) const
{
   return mu32_LastOverlayErrorAddress;
}
