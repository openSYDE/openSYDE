//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handle project save and load

   \class       stw_opensyde_core::C_OSCProjectFiler
   \brief       Handle project save and load

   (See .cpp file for full description)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCPROJECTFILERH
#define C_OSCPROJECTFILERH

/* -- Includes ------------------------------------------------------------- */
#include "stwtypes.h"
#include "CSCLString.h"
#include "C_OSCProject.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OSCProjectFiler
{
public:
   static stw_types::sint32 h_Save(C_OSCProject & orc_Project, const stw_scl::C_SCLString & orc_Path,
                                   const stw_scl::C_SCLString & orc_OpenSYDEVersion);
   static stw_types::sint32 h_Load(C_OSCProject & orc_Project, const stw_scl::C_SCLString & orc_Path);

private:
   static stw_types::sint32 m_SaveInternal(C_OSCProject & orc_Project, const stw_scl::C_SCLString & orc_Path,
                                           const stw_scl::C_SCLString & orc_OpenSYDEVersion, const bool oq_New);
};

/* -- Extern Global Variables ---------------------------------------------- */
}

#endif
