//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Utility class to store pointers to any device info storage (implementation)

   Utility class to store pointers to any device info storage

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     19.02.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_SyvUpDeviceInfo.h"

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

   \created     19.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SyvUpDeviceInfo::C_SyvUpDeviceInfo(void) :
   pc_STWDevice(NULL),
   pc_OSYDevice(NULL)
{
}
