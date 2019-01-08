//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Handle user project data

   This class handles all actions concerning the user project.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     04.07.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "TGLUtils.h"
#include "C_OSCProject.h"
#include "CSCLChecksums.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_opensyde_core;
using namespace stw_types;
using namespace std;
using namespace stw_scl;
using namespace stw_errors;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Constructor

   Initialize all elements with default values

   \created     05.07.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OSCProject::C_OSCProject(void) :
   c_CreationTime(C_SCLDateTime::Now()),
   c_ModificationTime(C_SCLDateTime::Now()),
   c_OpenSYDEVersion("0.01r0"),
   c_Template("Empty project"),
   c_Version("0.01r0b0")
{
   stw_tgl::TGL_GetSystemUserName(c_Author);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     05.07.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OSCProject::~C_OSCProject(void)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with initial [in] value and result [out] value

   \created     21.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_OSCProject::CalcHash(uint32 & oru32_HashValue) const
{
   C_SCLChecksums::CalcCRC32(this->c_Author.c_str(), this->c_Author.Length(), oru32_HashValue);
   C_SCLChecksums::CalcCRC32(this->c_OpenSYDEVersion.c_str(), this->c_OpenSYDEVersion.Length(), oru32_HashValue);
   // no need to check c_CreationTime & c_ModificationTime
   C_SCLChecksums::CalcCRC32(this->c_Template.c_str(), this->c_Template.Length(), oru32_HashValue);
   C_SCLChecksums::CalcCRC32(this->c_Version.c_str(), this->c_Version.Length(), oru32_HashValue);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get modification date and time as formatted string

   Format: "DD.MM.JJJJ HH:MM"

   \param[in,out] orc_Time Time to output

   \return
   Formatted date

   \created     05.07.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SCLString C_OSCProject::h_GetTimeFormatted(const C_SCLDateTime & orc_Time)
{
   C_SCLString c_StrTime = orc_Time.DateTimeToString();

   //Remove seconds
   return c_StrTime.SubString(1, 16);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Convert string to time

   Expected format:
   - days months years hours minutes as numeric values
   - separated by "." " " or ":" (or any mixture of these)
   - example: "DD.MM.JJJJ HH:MM"

   \param[in] orc_Str String to be evaluated

   \return
   Time (current time of orc_Str is invalid)

   \created     05.07.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SCLDateTime C_OSCProject::h_GetTimeOfString(const C_SCLString & orc_Str)
{
   C_SCLDateTime c_Retval;
   bool q_Err = true;

   SCLDynamicArray<C_SCLString> c_Dyn;
   orc_Str.Tokenize(". :", c_Dyn);
   if (c_Dyn.GetLength() == 5)
   {
      try
      {
         c_Retval.mu16_Day   = static_cast<uint16>(c_Dyn[0].ToInt());
         c_Retval.mu16_Month = static_cast<uint16>(c_Dyn[1].ToInt());
         c_Retval.mu16_Year  = static_cast<uint16>(c_Dyn[2].ToInt());
         c_Retval.mu16_Hour  = static_cast<uint16>(c_Dyn[3].ToInt());
         c_Retval.mu16_Minute = static_cast<uint16>(c_Dyn[4].ToInt());
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
      c_Retval = C_SCLDateTime::Now();
   }
   return c_Retval;
}
