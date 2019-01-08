//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Project system view dashboard user settings (implementation)

   Project system view dashboard user settings

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     10.07.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_UsSystemViewDashboard.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     10.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_UsSystemViewDashboard::C_UsSystemViewDashboard(void) :
   q_TornOff(false),
   s32_MainWindowTabIndex(-1),
   c_TornOffWindowPosition(0, 0),
   c_TornOffWindowSize(1000, 800),
   q_TornOffWindowMinimized(false),
   q_TornOffWindowMaximized(false),
   sn_SceneZoom(100),
   c_ScenePos(0, 0)
{
}
