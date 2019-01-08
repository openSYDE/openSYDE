//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Command for grouping move commands (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     24.11.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SEBUNOMOVECOMMANDGROUP_H
#define C_SEBUNOMOVECOMMANDGROUP_H

/* -- Includes ------------------------------------------------------------- */

#include <QUndoCommand>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SebUnoMoveCommandGroup :
   public QUndoCommand
{
public:
   C_SebUnoMoveCommandGroup(QUndoCommand * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
