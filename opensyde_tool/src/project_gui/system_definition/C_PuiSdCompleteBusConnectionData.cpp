//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Node connection data for complete reconstruction (implementation)

   Node connection data for complete reconstruction

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     23.11.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_PuiSdCompleteBusConnectionData.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
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

   \created     23.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_PuiSdCompleteBusConnectionData::C_PuiSdCompleteBusConnectionData() :
   u64_NodeID(0),
   u64_BusID(0)
{
}
