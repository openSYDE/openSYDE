//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class to describe a context menu entry (header)

   Class to describe a context menu entry (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDCONTEXTMENUENTRY_H
#define C_SDCONTEXTMENUENTRY_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QString>

#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdContextMenuEntry
{
public:
   C_SdContextMenuEntry();

   QString c_Name;              ///< Displayed name
   bool q_Active;               ///< Indicator this item should be clickable or if it is just informational
   bool q_Heading;              ///< Indicator this item should be a context manu heading
   stw_types::sint32 s32_Index; ///< Index returned via the context menu manager if this entry is triggered
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
