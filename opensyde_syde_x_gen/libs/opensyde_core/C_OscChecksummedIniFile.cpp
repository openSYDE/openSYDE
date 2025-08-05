//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Checksummed INI file class

   For details cf. documentation in .h file.

   \copyright   Copyright 2009 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_OscChecksummedIniFile.hpp"
#include "C_SclChecksums.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::scl;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
static const uint16_t mu16_CHECKSUM_VERSION = 2U;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   constructor

   Open and parse INI file.
   Does NOT check the checksum.

   \param[in]     orc_FileName     path to INI file
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscChecksummedIniFile::C_OscChecksummedIniFile(const C_SclString & orc_FileName) :
   C_SclIniFile(orc_FileName)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   destructor

   clean up ...
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscChecksummedIniFile::~C_OscChecksummedIniFile(void)
{
}

//----------------------------------------------------------------------------------------------------------------------

uint16_t C_OscChecksummedIniFile::m_CalcCheckSum(const bool oq_Fast)
{
   uint32_t u32_NumSections;
   uint16_t u16_CheckSumCalc;
   C_SclStringList c_SectionList;
   C_SclStringList c_DirectivesValues;

   this->ReadSections(&c_SectionList);
   u32_NumSections = static_cast<uint16_t>(c_SectionList.GetCount());
   u16_CheckSumCalc = 58453U;
   for (uint32_t u32_Section = 0U; u32_Section < u32_NumSections; u32_Section++)
   {
      if (c_SectionList.Strings[u32_Section] != "INISAFE")
      {
         //if "fast": do NOT append
         uint32_t u32_NumDirectives;
         ReadSectionValues(c_SectionList.Strings[u32_Section], &c_DirectivesValues, !oq_Fast);
         u32_NumDirectives = c_DirectivesValues.GetCount();
         for (uint32_t u32_Directive = 0U; u32_Directive < u32_NumDirectives; u32_Directive++)
         {
            const C_SclString & rc_String = c_DirectivesValues.Strings[u32_Directive];
            for (uint32_t u32_Byte = 0U; u32_Byte < rc_String.Length(); u32_Byte++)
            {
               uint8_t u8_Value;
               u8_Value = static_cast<uint8_t>(rc_String.c_str()[u32_Byte]);
               u8_Value = static_cast<uint8_t>((static_cast<uint8_t>(u8_Value << 3U)) + (u8_Value >> 5U));
               C_SclChecksums::CalcCRC16STW(&u8_Value, 1U, u16_CheckSumCalc);
            }
         }
      }
   }
   return u16_CheckSumCalc;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Verify checksum of INI file data

   Calculates checksum over INI file contents and compares it agains the checksum
    stored in the ini file data.

   \return
   true   checksums match
   false  checksums do not match
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscChecksummedIniFile::CheckCheckSum(void)
{
   uint16_t u16_CheckSumCalc;
   uint16_t u16_CheckSumIni1;
   uint16_t u16_CheckSumIni2;
   uint16_t u16_Version;
   bool q_Match = false;

   u16_Version      = this->ReadUint16("INISAFE", "VERSION",     1U);
   u16_CheckSumIni1 = this->ReadUint16("INISAFE", "CHECKSUM",    0U);
   u16_CheckSumIni2 = this->ReadUint16("INISAFE", "CHECKSUM_V2", 0U);
   if (u16_Version == 1U)
   {
      u16_CheckSumCalc = this->m_CalcCheckSum(false);
      if (u16_CheckSumIni1 == u16_CheckSumCalc)
      {
         q_Match = true;
      }
   }
   else
   {
      u16_CheckSumCalc = this->m_CalcCheckSum(true);
      if (u16_CheckSumIni2 == u16_CheckSumCalc)
      {
         q_Match = true;
      }
      else
      {
         //check V1 checksum (maybe file was created by an old version of this class)
         u16_CheckSumCalc = this->m_CalcCheckSum(false);
         if (u16_CheckSumIni1 == u16_CheckSumCalc)
         {
            q_Match = true;
         }
      }
   }
   return q_Match;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Recalculate checksum

   Calculated checksum over INI file contents and stores it within the contents.
   Note: Does not write to the file immediately; this is only done in the destructor of the base class
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscChecksummedIniFile::UpdateCheckSum(void)
{
   uint16_t u16_CheckSumCalcV1;
   uint16_t u16_CheckSumCalcV2;

   u16_CheckSumCalcV1 = m_CalcCheckSum(false);
   u16_CheckSumCalcV2 = m_CalcCheckSum(true);
   //write both, old and new checksums to stay compatible to old tools
   if (this->SectionExists("INISAFE") == true)
   {
      this->EraseSection("INISAFE"); //clean up ...
   }
   this->WriteInteger("INISAFE", "VERSION",  mu16_CHECKSUM_VERSION);
   //write V1 checksum for compatibility with older applications:
   this->WriteInteger("INISAFE", "CHECKSUM", u16_CheckSumCalcV1);
   this->WriteInteger("INISAFE", "CHECKSUM_V2", u16_CheckSumCalcV2);
}

//----------------------------------------------------------------------------------------------------------------------
