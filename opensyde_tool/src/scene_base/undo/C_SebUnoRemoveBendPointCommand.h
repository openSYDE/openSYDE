//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Remove bend point for line undo command (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     25.11.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SEBUNOTOPREMOVEBENDPOINTCOMMAND_H
#define C_SEBUNOTOPREMOVEBENDPOINTCOMMAND_H

/* -- Includes ------------------------------------------------------------- */

#include "C_SebUnoBendPointBaseCommand.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SebUnoRemoveBendPointCommand :
   public C_SebUnoBendPointBaseCommand
{
public:
   C_SebUnoRemoveBendPointCommand(QGraphicsScene * const opc_Scene, const std::vector<stw_types::uint64> & orc_IDs,
                                  const QPointF & orc_ScenePosition, QUndoCommand * const opc_Parent = NULL);
   virtual ~C_SebUnoRemoveBendPointCommand(void);
   virtual void undo(void) override;
   virtual void redo(void) override;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
