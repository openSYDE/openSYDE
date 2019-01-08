//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Checksummed INI file class

   For details cf. documentation in .h file.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     16.04.2009  STW/A.Stangl (original implementation as CDLSecureINIFile)
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_OSCChecksummedIniFile.h"
#include "CSCLChecksums.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_scl;
using namespace stw_opensyde_core;

/* -- Module Global Constants ---------------------------------------------- */
static const uint16 mu16_CHECKSUM_VERSION = 2U;

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   constructor

   Open and parse INI file.
   Does NOT check the checksum.

   \param[in]     orc_FileName     path to INI file

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_OSCChecksummedIniFile::C_OSCChecksummedIniFile(const C_SCLString & orc_FileName) :
   C_SCLIniFile(orc_FileName)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   destructor

   clean up ...

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_OSCChecksummedIniFile::~C_OSCChecksummedIniFile(void)
{
}

//---------------------------------------------------------------------------

uint16 C_OSCChecksummedIniFile::m_CalcCheckSum(const bool oq_Fast)
{
   uint32 u32_NumSections;
   uint16 u16_CheckSumCalc;
   C_SCLStringList c_SectionList;
   C_SCLStringList c_DirectivesValues;

   this->ReadSections(&c_SectionList);
   u32_NumSections = static_cast<uint16>(c_SectionList.GetCount());
   u16_CheckSumCalc = 58453U;
   for (uint32 u32_Section = 0U; u32_Section < u32_NumSections; u32_Section++)
   {
      if (c_SectionList.Strings[u32_Section] != "INISAFE")
      {
         //if "fast": do NOT append
         uint32 u32_NumDirectives;
         ReadSectionValues(c_SectionList.Strings[u32_Section], &c_DirectivesValues, !oq_Fast);
         u32_NumDirectives = c_DirectivesValues.GetCount();
         for (uint32 u32_Directive = 0U; u32_Directive < u32_NumDirectives; u32_Directive++)
         {
            const C_SCLString & rc_String = c_DirectivesValues.Strings[u32_Directive];
            for (uint32 u32_Byte = 0U; u32_Byte < rc_String.Length(); u32_Byte++)
            {
               uint8 u8_Value;
               u8_Value = static_cast<uint8>(rc_String.c_str()[u32_Byte]);
               u8_Value = static_cast<uint8>((static_cast<uint8>(u8_Value << 3)) + (u8_Value >> 5));
               C_SCLChecksums::CalcCRC16STW(&u8_Value, 1U, u16_CheckSumCalc);
            }
         }
      }
   }
   return u16_CheckSumCalc;
}

//-----------------------------------------------------------------------------
/*!
   \brief    Verify checksum of INI file data

   Calculates checksum over INI file contents and compares it agains the checksum
    stored in the ini file data.

   \return
   true   checksums match
   false  checksums do not match

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
bool C_OSCChecksummedIniFile::CheckCheckSum(void)
{
   uint16 u16_CheckSumCalc;
   uint16 u16_CheckSumINI1;
   uint16 u16_CheckSumINI2;
   uint16 u16_Version;

   u16_Version      = this->ReadUint16("INISAFE", "VERSION",     1U);
   u16_CheckSumINI1 = this->ReadUint16("INISAFE", "CHECKSUM",    0U);
   u16_CheckSumINI2 = this->ReadUint16("INISAFE", "CHECKSUM_V2", 0U);
   if (u16_Version == 1U)
   {
      u16_CheckSumCalc = this->m_CalcCheckSum(false);
      if (u16_CheckSumINI1 == u16_CheckSumCalc)
      {
         return true;
      }
   }
   else
   {
      u16_CheckSumCalc = this->m_CalcCheckSum(true);
      if (u16_CheckSumINI2 == u16_CheckSumCalc)
      {
         return true;
      }
      //check V1 checksum (maybe file was created by an old version of this class)
      u16_CheckSumCalc = this->m_CalcCheckSum(false);
      if (u16_CheckSumINI1 == u16_CheckSumCalc)
      {
         return true;
      }
   }
   return false;
}

//-----------------------------------------------------------------------------
/*!
   \brief    Recalculate checksum

   Calculated checksum over INI file constets and stores it within the contents.
   Note: Does not write to the file immediately; this is only done in the destructor of the base class

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
void C_OSCChecksummedIniFile::UpdateCheckSum(void)
{
   uint16 u16_CheckSumCalcV1;
   uint16 u16_CheckSumCalcV2;

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

//---------------------------------------------------------------------------
