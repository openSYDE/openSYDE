//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       header for class C_HexFile

   defines the structure of class C_HexFile

   \copyright   Copyright 2000 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef CHEXFILEHPP
#define CHEXFILEHPP

#include <cstdio> //for "FILE"
#include "stwtypes.hpp"
#include "C_SclDynamicArray.hpp"

namespace stw
{
namespace hex_file
{
//----------------------------------------------------------------------------------------------------------------------
// defines
//----------------------------------------------------------------------------------------------------------------------
//maybe this will be a part of a Borland library:
#ifndef STWHEXFILE_PACKAGE
#ifdef __BORLANDC__
#define STWHEXFILE_PACKAGE __declspec(package)
#else
#define STWHEXFILE_PACKAGE
#endif
#endif

// Error and Warnings for LoadFromFile
const uint32_t NO_ERR                = 0x00000000UL;
const uint32_t WRN_NO_EOF_RECORD     = 0x10000000UL;
const uint32_t WRN_RECORD_OVERLAY    = 0x20000000UL;
const uint32_t ERR_HEXLINE_SYNTAX    = 0x80000000UL;
const uint32_t ERR_HEXLINE_CHECKSUM  = 0x90000000UL;
const uint32_t ERR_HEXLINE_COMMAND   = 0xA0000000UL;
const uint32_t ERR_NO_DATA           = 0xB0000000UL;
const uint32_t ERR_NOT_ENOUGH_MEMORY = 0xE0000000UL;
const uint32_t ERR_CANT_OPEN_FILE    = 0xF0000000UL;

const uint32_t ERR_MASK              = 0xF0000000UL;

//----------------------------------------------------------------------------------------------------------------------
// data structures
//----------------------------------------------------------------------------------------------------------------------

///Element for one line of a hex file (will be created in a dynamic ring buffer)
struct STWHEXFILE_PACKAGE T_HexLine
{
   T_HexLine * pt_Prev;            ///< previous element
   T_HexLine * pt_Next;            ///< next Element
   uint32_t u32_XAdr;     ///< extended addresse of hex file line
   uint8_t * pu8_HexLine; ///< binary data contained in hex line (without leading ":")
};

//----------------------------------------------------------------------------------------------------------------------
// class definition, prototypes
//----------------------------------------------------------------------------------------------------------------------
///Container for on block of memory dump
class STWHEXFILE_PACKAGE C_HexDataDumpBlock
{
public:
   uint32_t u32_AddressOffset;
   stw::scl::C_SclDynamicArray<uint8_t> au8_Data;
};

///Array of containers for memory dump data
class STWHEXFILE_PACKAGE C_HexDataDump
{
public:
   stw::scl::C_SclDynamicArray<C_HexDataDumpBlock> at_Blocks;
};

///Handles reading and writing of hex files
//lint -sem(stw::hex_file::C_HexFile::InitHexFile,initializer)
class STWHEXFILE_PACKAGE C_HexFile
{
private:
   C_HexFile(const C_HexFile & orc_Source);               ///< not implemented -> prevent copying
   C_HexFile & operator = (const C_HexFile & orc_Source); ///< not implemented -> prevent assignment

public:
   C_HexFile(void);
   virtual ~C_HexFile(void);

   void Clear(void);
   uint32_t LoadFromFile(const char_t * const opcn_FileName);
   uint32_t SaveToFile(const char_t * const opcn_FileName);

   //Reformat hex file (uses a memory dump internally); only use if you know there are no bigger gaps in the hex file
   //                   data)
   uint32_t OptimizeLinear(const uint32_t ou32_RecSize, const int32_t os32_FillFlag,
                                    const uint8_t ou8_FillPattern);
   //Reformat hex files (using C_HexDataDump internally; so no RAM penalty for gaps within hex file)
   uint32_t Optimize(const uint32_t ou32_RecSize);

   //Set data pointer to first element (T_HexLine)
   //      and return pointer to data of current hex line.
   // Error: NULL pointer
   const uint8_t * LineInit(void);

   //Get pointer to binary data, destination address and size information
   const uint8_t * NextBinData(uint32_t & oru32_Address, uint8_t & oru8_Size);

   //Get pointer to current hex line and advance data pointer to next line.
   //After last line sets data pointer to NULL
   // Error: NULL pointer
   const uint8_t * NextLine(void);

   //Get pointer to data of current hex line as string and advance data pointer to next line.
   //After last line sets data pointer to NULL
   // Error: NULL pointer
   const char_t * NextLineString(void);

   //Get total number of hex file lines
   uint32_t LineCount(void) const;
   //Get lowest occupied address in hex file
   uint32_t MinAdr(void) const;
   //Get highest occupied address in hex file
   uint32_t MaxAdr(void) const;

   uint32_t ByteCount(void) const;

   //check for overlapping memory
   uint32_t Validate(void);

   uint32_t GetXAdrActLine(uint32_t & oru32_XAdr) const;

   //create hex file data from linear binary image
   uint32_t CreateHexFile(const uint16_t * const opu16_BinImage, const uint32_t ou32_Offset,
                                   const uint32_t ou32_Size, const uint32_t ou32_RecSize);
   //create hex file data from image that only contains used data
   uint32_t CreateHexFile(const C_HexDataDump & orc_Dump, const uint32_t ou32_RecSize);

   const C_HexDataDump * GetDataDump(uint32_t & oru32_ErrorResult);

   int32_t GetDataByAddress(const uint32_t ou32_Address, uint16_t & oru16_NumBytes,
                                      uint8_t * const opu8_Data);
   int32_t FindPattern(uint32_t & oru32_Address, const uint8_t ou8_PatternLength,
                                 const uint8_t * const opu8_Pattern);

   //call on WRN_RECORD_OVERLAY to find out offending address:
   uint32_t GetLastOverlayErrorAddress(void) const;

protected:
   uint32_t mu32_MinAdr; // Lowest occupied address in hex file
   uint32_t mu32_MaxAdr; // Highest occupied address in hex file
   uint32_t mu32_AdrOffs;
   uint32_t mu32_LineCount;
   uint32_t mu32_LineCountNew; //Total number of hex file lines
   uint32_t mu32_NumRawBytes;
   uint8_t mu8_MaxRecordLength; // Maximum detected record length when parsing hex file; set by LoadFromFile
   const T_HexLine * mpt_DataEntry;      // Start of dynamic ring buffer containing all hex lines
   const T_HexLine * mpt_HexData;        // current position in ring buffer
   T_HexLine * mpt_Prev;                 // previous element of ring buffer

   C_HexDataDump mc_Dump; //data in the form of an array of memory dump blocks
   bool mq_DumpIsDirty;   //flags whether dump has been compromised by changing hex data contents since it was
                          // last built
   uint32_t mu32_LastOverlayErrorAddress;

   void m_InitHexFile(void);
   uint32_t m_GetFileType(std::FILE * const opt_File, int32_t & ors32_FileType) const;
   uint32_t m_LoadIntelHex(std::FILE * const opt_File);
   uint32_t m_LoadSRecord(std::FILE * const opt_File);
   uint32_t m_CopyHex2Mem(uint16_t * opu16_BinImage, const uint32_t ou32_Offset);
   uint32_t m_CloseRecord(char_t * const opcn_Record, bool & orq_RecordOpen);
   static const char_t * mh_HexLineString(const uint8_t * const opu8_HexLine);
   uint32_t m_GetIHexCommand(const char_t * const opcn_String,
                                      uint8_t & oru8_Command) const;
   uint32_t m_GetSRecordType(const char_t * const opcn_String, uint8_t & oru8_RecordType)
   const;
   uint32_t m_GetIntelAddress(const char_t * const opcn_String, const uint8_t ou8_Command,
                                       uint32_t & oru32_Adr) const;
   uint32_t m_GetSRecordAddress(const char_t * const opcn_String,
                                         const uint8_t ou8_RecordType, uint32_t & oru32_Adr) const;
   void             m_SetEOFPtr(void);
   void             m_SetDataPtr(const uint32_t ou32_Adr);
   uint32_t m_SetXAdrPtr(const uint32_t ou32_Adr);
   uint32_t m_AddHexLine(const char_t * const opcn_String);
   void             m_RemoveFirst(void);
   uint32_t m_ConvOffs16To32(char_t * const opcn_String) const;
   uint32_t m_ConvRec16ToRec32(char_t * const opcn_String);
   uint32_t m_SetOffset(const uint32_t ou32_Adr);
   static uint32_t mh_CalcCheck(char_t * const opcn_String);
   uint32_t m_CopyData(const char_t * opcn_String, T_HexLine * const opt_HexLine,
                                const uint32_t ou32_Length);
   static uint32_t mh_GetByte(const char_t * const opcn_String, const uint32_t ou32_Index,
                                       uint8_t & oru8_Data);
   static void mh_SetByte(char_t * const opcn_String, const uint32_t ou32_Index,
                          const uint8_t ou8_Byte);
   static uint32_t mh_GetWord(const char_t * const opcn_String, const uint32_t ou32_Index,
                                       uint32_t & oru32_Word);
   static void mh_SetWord(char_t * const opcn_String, const uint32_t ou32_Index,
                          const uint32_t ou32_Data);

   static int32_t mh_FindPattern(const uint8_t * const opu8_Buffer,
                                           const uint8_t * const opu8_Pattern,
                                           const uint32_t ou32_BufSize,
                                           const uint16_t ou16_PatternLength);
};
}
}

//----------------------------------------------------------------------------------------------------------------------

#endif
