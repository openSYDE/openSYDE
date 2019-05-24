//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE Core gettext utilities

   cf. .cpp file header for details

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GTGETTEXTH
#define C_GTGETTEXTH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "CSCLString.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GtGetText
{
private:
   static bool mhq_Initialized;

public:
   static stw_types::sint32 h_Initialize(const stw_scl::C_SCLString & orc_BasePath,
                                         const stw_scl::C_SCLString & orc_MoFileName);
   static stw_types::sint32 h_SetLanguage(const stw_scl::C_SCLString & orc_Language);
   static const stw_types::charn * h_GetText(const stw_types::charn * const opcn_MessageID);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}

#endif
