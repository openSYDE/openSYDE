//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Move sub line undo command (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     22.11.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SEBUNOTOPMOVESUBLINECOMMAND_H
#define C_SEBUNOTOPMOVESUBLINECOMMAND_H

/* -- Includes ------------------------------------------------------------- */

#include "C_SebUnoBaseCommand.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SebUnoMoveSubLineCommand :
   public C_SebUnoBaseCommand
{
public:
   C_SebUnoMoveSubLineCommand(QGraphicsScene * const opc_Scene, const std::vector<stw_types::uint64> & orc_IDs,
                              const stw_types::sint32 & ors32_SubLineID, const QPointF & orc_PositionDifference,
                              QUndoCommand * const opc_Parent = NULL);
   virtual ~C_SebUnoMoveSubLineCommand();
   virtual void undo(void) override;
   virtual void redo(void) override;

private:
   const stw_types::sint32 ms32_SubLineID;
   const QPointF mc_PositionDifference;

   void m_UndoSingle(QGraphicsItem * const opc_Item) const;
   void m_RedoSingle(QGraphicsItem * const opc_Item) const;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
