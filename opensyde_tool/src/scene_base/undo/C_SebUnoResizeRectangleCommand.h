//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Resize rectangle undo command (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     22.11.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SEBUNORESIZERECTANGLECOMMAND_H
#define C_SEBUNORESIZERECTANGLECOMMAND_H

/* -- Includes ------------------------------------------------------------- */

#include <QPointF>
#include <QSizeF>
#include "C_SebUnoBaseCommand.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SebUnoResizeRectangleCommand :
   public C_SebUnoBaseCommand
{
public:
   C_SebUnoResizeRectangleCommand(QGraphicsScene * const opc_Scene, const std::vector<stw_types::uint64> & orc_IDs,
                                  const QPointF & orc_OldPos, const QSizeF & orc_OldSize, const QPointF & orc_NewPos,
                                  const QSizeF & orc_NewSize, QUndoCommand * const opc_Parent = NULL);
   virtual ~C_SebUnoResizeRectangleCommand();
   virtual void undo(void) override;
   virtual void redo(void) override;

private:
   const QPointF mc_OldPos;
   const QSizeF mc_OldSize;
   const QPointF mc_NewPos;
   const QSizeF mc_NewSize;

   void m_UndoSingle(QGraphicsItem * const opc_Item) const;
   void m_RedoSingle(QGraphicsItem * const opc_Item) const;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
