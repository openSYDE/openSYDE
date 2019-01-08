//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Command for grouping move commands (implementation)

   Command for grouping move commands

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     24.11.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_SebUnoMoveCommandGroup.h"

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

   \created     24.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SebUnoMoveCommandGroup::C_SebUnoMoveCommandGroup(QUndoCommand * const opc_Parent) :
   QUndoCommand("Move drawing element(s)", opc_Parent)
{
}
