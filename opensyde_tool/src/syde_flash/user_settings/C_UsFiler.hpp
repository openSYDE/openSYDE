//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handle save'n load for user settings (header)

   Handle save'n load for user settings (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_USFILER_HPP
#define C_USFILER_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_UsHandler.hpp"
#include "stwtypes.hpp"
#include "C_SclIniFile.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_UsFiler
{
public:
   static int32_t h_Save(const C_UsHandler & orc_UserSettings, const QString & orc_Path);
   static int32_t h_Load(C_UsHandler & orc_UserSettings, const QString & orc_Path);

private:
   C_UsFiler(void);

   static void mh_SaveProjectIndependentSection(const C_UsHandler & orc_UserSettings, stw::scl::C_SclIniFile & orc_Ini);
   static void mh_LoadProjectIndependentSection(C_UsHandler & orc_UserSettings, stw::scl::C_SclIniFile & orc_Ini);

   static C_UsHandler::E_CanDllType mh_GetDllTypeFromString(const QString & orc_Value);
   static QString mh_GetStringFromDllType(const C_UsHandler::E_CanDllType oe_Value);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
