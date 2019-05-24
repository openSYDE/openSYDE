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
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_UsSystemViewDashboard
{
public:
   C_UsSystemViewDashboard(void);

   bool q_TornOff; ///< True:  dashboard torn off
   ///< False: dashboard integrated into main window
   stw_types::sint32 s32_MainWindowTabIndex; ///< Tab index (-1 if invalid)
   ///< Only valid if not torn off
   QPoint c_TornOffWindowPosition; ///< Torn off window position
   ///< Only valid if torn off
   QSize c_TornOffWindowSize; ///< Torn off window size
   ///< Only valid if torn off
   bool q_TornOffWindowMinimized; ///< Torn off window minimized flag
   ///< Only valid if torn off
   bool q_TornOffWindowMaximized; ///< Torn off window maximized flag
   ///< Only valid if torn off
   stw_types::sintn sn_SceneZoom;
   QPoint c_ScenePos;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
