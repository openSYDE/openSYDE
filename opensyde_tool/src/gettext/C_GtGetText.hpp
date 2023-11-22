//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE Core gettext utilities

   cf. .cpp file header for details

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GTGETTEX_HPP
#define C_GTGETTEX_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SclString.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GtGetText
{
private:
   static bool mhq_Initialized;

public:
   static int32_t h_Initialize(const stw::scl::C_SclString & orc_BasePath,
                               const stw::scl::C_SclString & orc_MoFileName);
   static int32_t h_SetLanguage(const stw::scl::C_SclString & orc_Language);
   static const char_t * h_GetText(const char_t * const opcn_MessageId);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
}

#endif
