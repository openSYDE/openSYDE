//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Handle user project data

   \class       stw_opensyde_core::C_OSCProject
   \brief       Handle user project data

   This class handles all actions concerning the user project.
   (See .cpp file for full description)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     04.07.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OSCPROJECTH
#define C_OSCPROJECTH

/* -- Includes ------------------------------------------------------------- */
#include "stwtypes.h"
#include "CSCLDateTime.h"
#include "CSCLString.h"
#include "C_OSCXMLParser.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OSCProject
{
public:
   C_OSCProject(void);
   virtual ~C_OSCProject(void);

   void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   static stw_scl::C_SCLString h_GetTimeFormatted(const stw_scl::C_SCLDateTime & orc_Time);
   static stw_scl::C_SCLDateTime h_GetTimeOfString(const stw_scl::C_SCLString & orc_Str);

   stw_scl::C_SCLString c_Author;             ///< Author
   stw_scl::C_SCLDateTime c_CreationTime;     ///< Time of last known modification
   stw_scl::C_SCLDateTime c_ModificationTime; ///< Time of last known modification
   stw_scl::C_SCLString c_OpenSYDEVersion;    ///< openSYDE version used to save this project
   stw_scl::C_SCLString c_Template;           ///< Template this project is based on
   stw_scl::C_SCLString c_Version;            ///< project version

protected:
   static stw_types::sint32 mh_CheckTimeFormat(const stw_scl::C_SCLString & orc_Time);
};

/* -- Extern Global Variables ---------------------------------------------- */
}

#endif
