//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Project system view dashboard user settings (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_USSYSTEMVIEWDASHBOARD_H
#define C_USSYSTEMVIEWDASHBOARD_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QSize>
#include <QPoint>
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_UsSystemViewDashboard
{
public:
   C_UsSystemViewDashboard(void);

   bool q_TornOff; ///< True:  dashboard torn off
   ///< False: dashboard integrated into main window
   QPoint c_TornOffWindowPosition; ///< Torn off window position
   ///< Only valid if torn off
   QSize c_TornOffWindowSize; ///< Torn off window size
   ///< Only valid if torn off
   bool q_TornOffWindowMinimized; ///< Torn off window minimized flag
   ///< Only valid if torn off
   bool q_TornOffWindowMaximized; ///< Torn off window maximized flag
   ///< Only valid if torn off
   int32_t s32_SceneZoom;
   QPoint c_ScenePos;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
