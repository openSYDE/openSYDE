//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Class to describe a context menu entry (implementation)

   Class to describe a context menu entry

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     22.09.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_SdContextMenuEntry.h"

/* -- Used Namespaces ------------------------------------------------------ */

using namespace stw_opensyde_gui;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     22.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdContextMenuEntry::C_SdContextMenuEntry()
{
   c_Name = "";
   q_Active = true;
   q_Heading = false;
   s32_Index = -1;
}
