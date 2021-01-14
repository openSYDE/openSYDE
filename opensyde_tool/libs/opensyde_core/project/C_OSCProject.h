//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handle user project data

   \class       stw_opensyde_core::C_OSCProject
   \brief       Handle user project data

   This class handles all actions concerning the user project.
   (See .cpp file for full description)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCPROJECTH
#define C_OSCPROJECTH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"
#include "CSCLDateTime.h"
#include "CSCLString.h"
#include "C_OSCXMLParser.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCProject
{
public:
   C_OSCProject(void);
   virtual ~C_OSCProject(void);

   void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   static stw_scl::C_SCLString h_GetTimeFormatted(const stw_scl::C_SCLDateTime & orc_Time);
   static stw_scl::C_SCLDateTime h_GetTimeOfString(const stw_scl::C_SCLString & orc_Str);

   stw_scl::C_SCLString c_Author;             ///< Author (person who created this project)
   stw_scl::C_SCLString c_Editor;             ///< Editor (person who last modified this project)
   stw_scl::C_SCLDateTime c_CreationTime;     ///< Time of creation
   stw_scl::C_SCLDateTime c_ModificationTime; ///< Time of last known modification
   stw_scl::C_SCLString c_OpenSYDEVersion;    ///< openSYDE version used to save this project
   stw_scl::C_SCLString c_Template;           ///< Template this project is based on
   stw_scl::C_SCLString c_Version;            ///< project version
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}

#endif
