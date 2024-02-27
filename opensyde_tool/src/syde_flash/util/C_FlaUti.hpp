//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       SydeFlash util class
   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_FLAUTI_HPP
#define C_FLAUTI_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QString>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_FlaUti
{
public:
   static QString h_ResolvePlaceholderVariables(const QString & orc_Path);
   static QString h_GetResolvedAbsolutePathFromExe(const QString & orc_Path);

private:
   C_FlaUti(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
