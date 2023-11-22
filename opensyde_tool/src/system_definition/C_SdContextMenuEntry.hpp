//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class to describe a context menu entry (header)

   Class to describe a context menu entry (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDCONTEXTMENUENTRY_HPP
#define C_SDCONTEXTMENUENTRY_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QString>

#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdContextMenuEntry
{
public:
   C_SdContextMenuEntry();

   QString c_Name;    ///< Displayed name
   bool q_Active;     ///< Indicator this item should be clickable or if it is just informational
   bool q_Heading;    ///< Indicator this item should be a context manu heading
   int32_t s32_Index; ///< Index returned via the context menu manager if this entry is triggered
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
