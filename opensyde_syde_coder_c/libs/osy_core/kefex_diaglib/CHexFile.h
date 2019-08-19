//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       header for class HexFile

   defines the structure of class HexFile

   \copyright   Copyright 2000 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef CHexfileH
#define CHexfileH

#include <cstdio> //for "FILE"
#include "stwtypes.h"
#include "SCLDynamicArray.h"

namespace stw_hex_file
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
const stw_types::uint32 NO_ERR                = 0x00000000UL;
const stw_types::uint32 WRN_NO_EOF_RECORD     = 0x10000000UL;
const stw_types::uint32 WRN_RECORD_OVERLAY    = 0x20000000UL;
const stw_types::uint32 ERR_HEXLINE_SYNTAX    = 0x80000000UL;
const stw_types::uint32 ERR_HEXLINE_CHECKSUM  = 0x90000000UL;
const stw_types::uint32 ERR_HEXLINE_COMMAND   = 0xA0000000UL;
const stw_types::uint32 ERR_NO_DATA           = 0xB0000000UL;
const stw_types::uint32 ERR_NOT_ENOUGH_MEMORY = 0xE0000000UL;
const stw_types::uint32 ERR_CANT_OPEN_FILE    = 0xF0000000UL;

const stw_types::uint32 ERR_MASK              = 0xF0000000UL;

//----------------------------------------------------------------------------------------------------------------------
// data structures
//----------------------------------------------------------------------------------------------------------------------

///Element fuer eine Zeile einer Hex-Datei (wird in einem dynamischen Ringpuffer angelegt)
struct STWHEXFILE_PACKAGE T_HexLine
{
  T_HexLine * pt_Prev;             ///< vorheriges Element
  T_HexLine * pt_Next;             ///< naechstes Element
  stw_types::uint32 u32_XAdr;      ///< Extended Adresse von Hex File Zeile
  stw_types::uint8 * pu8_HexLine;  ///< Daten Hex Zeile in Hex ohne ':'
};

//----------------------------------------------------------------------------------------------------------------------
// class definition, prototypes
//----------------------------------------------------------------------------------------------------------------------
///Container for on block of memory dump
class STWHEXFILE_PACKAGE C_HexDataDumpBlock
{
public:
   stw_types::uint32 u32_AddressOffset;
   stw_scl::SCLDynamicArray <stw_types::uint8> au8_Data;
};

///Array of containers for memory dump data
class STWHEXFILE_PACKAGE C_HexDataDump
{
public:
   stw_scl::SCLDynamicArray <C_HexDataDumpBlock> at_Blocks;
};

///Handles reading and writing of hex files
//lint -sem(stw_hex_file::C_HexFile::InitHexFile,initializer)
class STWHEXFILE_PACKAGE C_HexFile
{
private:
   C_HexFile(const C_HexFile & orc_Souce);               ///< not implemented -> prevent copying
   C_HexFile & operator = (const C_HexFile & orc_Souce); ///< not implemented -> prevent assignment
public:
   C_HexFile(void);
   virtual ~C_HexFile(void);

   void Clear(void);
   stw_types::uint32 LoadFromFile(const stw_types::charn * const opcn_FileName);
   stw_types::uint32 SaveToFile(const stw_types::charn * const opcn_FileName);

   //Reformat hex file (uses a memory dump internally); only use if you know there are no bigger gaps in the hex file
   //                   data)
   stw_types::uint32 OptimizeLinear(const stw_types::uint32 ou32_RecSize, const stw_types::sint32 os32_FillFlag,
                                    const stw_types::uint8 ou8_FillPattern);
   //Reformat hex files (using C_HexDataDump internally; so no RAM penalty for gaps within hex file)
   stw_types::uint32 Optimize(const stw_types::uint32 ou32_RecSize);

   //Datenzeiger auf ERSTES Element setzen (tHexLine)
   //      und Daten von aktueller Hex Zeile zurueck geben
   // Fehler: NULL Zeiger
   const stw_types::uint8 * LineInit(void);

   // get pointer to binary data, destination address and size information
   const stw_types::uint8 * NextBinData(stw_types::uint32 & oru32_Address, stw_types::uint8 & oru8_Size);

   //Daten von aktueller Hex Zeile zurueck geben
   //      und Datenzeiger auf die naechste Zeile stellen
   //      Nachdem die letzte Zeile aufgerufen ist -> Datenzeiger =NULL setzen
   // Fehler: NULL Zeiger
   const stw_types::uint8 * NextLine(void);

   // Daten von aktueller Hex Zeile als String-Zeiger zurueck geben
   //      und Datenzeiger auf die naechste Zeile stellen
   //      Nachdem die letzte Zeile aufgerufen ist -> Datenzeiger =NULL setzen
   // Fehler: NULL Zeiger
   const stw_types::charn * NextLineString(void);

   //Anzahl der Zeilen von Hex-File insgesamt
   stw_types::uint32 LineCount(void) const;
   //Minmale Adresse Hex-File
   stw_types::uint32 MinAdr(void) const;
   //Maximale Adresse Hex-File
   stw_types::uint32 MaxAdr(void) const;

   stw_types::uint32 ByteCount(void) const;

   //check for overlapping memory
   stw_types::uint32 Validate(void);

   stw_types::uint32 GetXAdrActLine(stw_types::uint32 & oru32_XAdr) const;

   //create hex file data from linear binary image
   stw_types::uint32 CreateHexFile(const stw_types::uint16 * const opu16_BinImage, const stw_types::uint32 ou32_Offset,
                                   const stw_types::uint32 ou32_Size, const stw_types::uint32 ou32_RecSize);
   //create hex file data from image that only contains used data
   stw_types::uint32 CreateHexFile(const C_HexDataDump & orc_Dump, const stw_types::uint32 ou32_RecSize);

   const C_HexDataDump * GetDataDump(stw_types::uint32 & oru32_ErrorResult);

   stw_types::sint32 GetDataByAddress(const stw_types::uint32 ou32_Address, stw_types::uint16 & oru16_NumBytes,
                                      stw_types::uint8 * const opu8_Data);
   stw_types::sint32 FindPattern(stw_types::uint32 & oru32_Address, const stw_types::uint8 ou8_PatternLength,
                                 const stw_types::uint8 * const opu8_Pattern);

   //call on WRN_RECORD_OVERLAY to find out offending address:
   stw_types::uint32 GetLastOverlayErrorAddress(void) const;

protected:
   stw_types::uint32 mu32_MinAdr;        // minimale Adresse Hex-Datei
   stw_types::uint32 mu32_MaxAdr;        // maximale Adresse Hex-Datei
   stw_types::uint32 mu32_AdrOffs;
   stw_types::uint32 mu32_LineCount;
   stw_types::uint32 mu32_LineCountNew;  // Anzahl der Hex Zeilen
   stw_types::uint32 mu32_NumRawBytes;
   stw_types::uint8 mu8_MaxRecordLength; // Maximum detected record length when parsing hex file; set by LoadFromFile
   const T_HexLine * mpt_DataEntry;         // Start von dynamischem Ringpuffer, der alle Hex Zeilen umfasst
   const T_HexLine * mpt_HexData;           // aktuelle Position in Ringpuffer
   T_HexLine * mpt_Prev;              // vorhergehendes Element von Ringpuffer

   C_HexDataDump mc_Dump; //data in the form of an array of memory dump blocks
   bool mq_DumpIsDirty;   //flags whether dump has been compromised by changing hex data contents since it was
                          // last built
   stw_types::uint32 mu32_LastOverlayErrorAddress;

   void InitHexFile(void);
   stw_types::uint32 GetFileType(std::FILE * const opt_File, stw_types::sint32 & ors32_FileType) const;
   stw_types::uint32 LoadIntelHex(std::FILE * const opt_File);
   stw_types::uint32 LoadSRecord(std::FILE * const opt_File);
   stw_types::uint32 CopyHex2Mem(stw_types::uint16 * opu16_BinImage, const stw_types::uint32 ou32_Offset);
   stw_types::uint32 CloseRecord(stw_types::charn * const opcn_Record, bool & orq_RecordOpen);
   static const stw_types::charn * HexLineString(const stw_types::uint8 * const opu8_HexLine);
   stw_types::uint32 GetIHexCommand(const stw_types::charn * const opcn_String, stw_types::uint8 & oru8_Command) const;
   stw_types::uint32 GetSRecordType(const stw_types::charn * const opcn_String, stw_types::uint8 & oru8_RecordType)
                                    const;
   stw_types::uint32 GetIntelAddress(const stw_types::charn * const opcn_String, const stw_types::uint8 ou8_Command,
                                     stw_types::uint32 & oru32_Adr) const;
   stw_types::uint32 GetSRecordAddress(const stw_types::charn * const opcn_String,
                                       const stw_types::uint8 ou8_RecordType, stw_types::uint32 & oru32_Adr) const;
   void             SetEOFPtr(void);
   void             SetDataPtr(const stw_types::uint32 ou32_Adr);
   stw_types::uint32 SetXAdrPtr(const stw_types::uint32 ou32_Adr);
   stw_types::uint32 AddHexLine(const stw_types::charn * const opcn_String);
   void             RemoveFirst(void);
   stw_types::uint32 ConvOffs16To32(stw_types::charn * const opcn_String) const;
   stw_types::uint32 ConvRec16ToRec32(stw_types::charn * const opcn_String);
   stw_types::uint32 SetOffset(const stw_types::uint32 ou32_Adr);
   static stw_types::uint32 CalcCheck(stw_types::charn * const opcn_String);
   stw_types::uint32 CopyData(const stw_types::charn * opcn_String, T_HexLine * const opt_HexLine,
                              const stw_types::uint32 ou32_Length);
   static stw_types::uint32 GetByte(const stw_types::charn * const opcn_String, const stw_types::uint32 ou32_Index,
                                    stw_types::uint8 & oru8_Data);
   static void SetByte(stw_types::charn * const opcn_String, const stw_types::uint32 ou32_Index,
                       const stw_types::uint8 ou8_Byte);
   static stw_types::uint32 GetWord(const stw_types::charn * const opcn_String, const stw_types::uint32 ou32_Index,
                                   stw_types::uint32 & oru32_Word);
   static void SetWord(stw_types::charn * const opcn_String, const stw_types::uint32 ou32_Index,
                       const stw_types::uint32 ou32_Data);

   static stw_types::sint32 prv_FindPattern(const stw_types::uint8 * const opu8_Buffer,
                                            const stw_types::uint8 * const opu8_Pattern,
                                            const stw_types::uint32 ou32_BufSize,
                                            const stw_types::uint16 ou16_PatternLength);
};

}

//----------------------------------------------------------------------------------------------------------------------

#endif
