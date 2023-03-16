//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handle user project data

   This class handles all actions concerning the user project.

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "TglUtils.hpp"
#include "C_OscProject.hpp"
#include "C_SclChecksums.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_core;

using namespace std;
using namespace stw::scl;
using namespace stw::errors;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Constructor

   Initialize all elements with default values
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscProject::C_OscProject(void) :
   c_CreationTime(C_SclDateTime::Now()),
   c_ModificationTime(C_SclDateTime::Now()),
   c_OpenSydeVersion("0.01r0"),
   c_Template("Empty project"),
   c_Version("0.01r0b0")
{
   stw::tgl::TglGetSystemUserName(c_Author);
   c_Editor = c_Author;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscProject::~C_OscProject(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with initial [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscProject::CalcHash(uint32_t & oru32_HashValue) const
{
   C_SclChecksums::CalcCRC32(this->c_Editor.c_str(), this->c_Editor.Length(), oru32_HashValue);
   C_SclChecksums::CalcCRC32(this->c_OpenSydeVersion.c_str(), this->c_OpenSydeVersion.Length(), oru32_HashValue);
   // no need to check c_Author, c_CreationTime & c_ModificationTime
   C_SclChecksums::CalcCRC32(this->c_Template.c_str(), this->c_Template.Length(), oru32_HashValue);
   C_SclChecksums::CalcCRC32(this->c_Version.c_str(), this->c_Version.Length(), oru32_HashValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get modification date and time as formatted string

   Format: "DD.MM.JJJJ HH:MM"

   \param[in,out] orc_Time Time to output

   \return
   Formatted date
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_OscProject::h_GetTimeFormatted(const C_SclDateTime & orc_Time)
{
   const C_SclString c_StrTime = orc_Time.DateTimeToString();

   //Remove seconds
   return c_StrTime.SubString(1, 16);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert string to time

   Expected format:
   - days months years hours minutes as numeric values
   - separated by "." " " or ":" (or any mixture of these)
   - example: "DD.MM.JJJJ HH:MM"

   \param[in] orc_Str String to be evaluated

   \return
   Time (current time of orc_Str is invalid)
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclDateTime C_OscProject::h_GetTimeOfString(const C_SclString & orc_Str)
{
   C_SclDateTime c_Retval;
   bool q_Err = true;

   C_SclDynamicArray<C_SclString> c_Dyn;
   orc_Str.Tokenize(". :", c_Dyn);
   if (c_Dyn.GetLength() == 5)
   {
      try
      {
         c_Retval.mu16_Day   = static_cast<uint16_t>(c_Dyn[0].ToInt());
         c_Retval.mu16_Month = static_cast<uint16_t>(c_Dyn[1].ToInt());
         c_Retval.mu16_Year  = static_cast<uint16_t>(c_Dyn[2].ToInt());
         c_Retval.mu16_Hour  = static_cast<uint16_t>(c_Dyn[3].ToInt());
         c_Retval.mu16_Minute = static_cast<uint16_t>(c_Dyn[4].ToInt());
         c_Retval.mu16_Second = 0U;
         q_Err = false;
      }
      catch (...)
      {
         q_Err = true;
      }
   }
   if (q_Err == true)
   {
      c_Retval = C_SclDateTime::Now();
   }
   return c_Retval;
}
