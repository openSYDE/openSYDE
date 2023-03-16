//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handle user project data

   \class       stw::opensyde_core::C_OscProject
   \brief       Handle user project data

   This class handles all actions concerning the user project.
   (See .cpp file for full description)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCPROJECTH
#define C_OSCPROJECTH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_SclDateTime.hpp"
#include "C_SclString.hpp"
#include "C_OscXmlParser.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscProject
{
public:
   C_OscProject(void);
   virtual ~C_OscProject(void);

   void CalcHash(uint32_t & oru32_HashValue) const;

   static stw::scl::C_SclString h_GetTimeFormatted(const stw::scl::C_SclDateTime & orc_Time);
   static stw::scl::C_SclDateTime h_GetTimeOfString(const stw::scl::C_SclString & orc_Str);

   stw::scl::C_SclString c_Author;             ///< Author (person who created this project)
   stw::scl::C_SclString c_Editor;             ///< Editor (person who last modified this project)
   stw::scl::C_SclDateTime c_CreationTime;     ///< Time of creation
   stw::scl::C_SclDateTime c_ModificationTime; ///< Time of last known modification
   stw::scl::C_SclString c_OpenSydeVersion;    ///< openSYDE version used to save this project
   stw::scl::C_SclString c_Template;           ///< Template this project is based on
   stw::scl::C_SclString c_Version;            ///< project version
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
}

#endif
