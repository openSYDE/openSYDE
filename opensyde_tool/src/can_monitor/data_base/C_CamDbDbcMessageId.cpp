//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Required indices for DBC message identification (implementation)

   Required indices for DBC message identification

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     17.01.2019  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_CamDbDbcMessageId.h"

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

   \created     17.01.2019  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_CamDbDbcMessageId::C_CamDbDbcMessageId(void) :
   u32_NodeIndex(0UL),
   q_MessageIsTx(false),
   u32_MessageIndex(0UL)
{
}
