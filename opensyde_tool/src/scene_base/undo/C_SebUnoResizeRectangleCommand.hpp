//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Resize rectangle undo command (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SEBUNORESIZERECTANGLECOMMAND_HPP
#define C_SEBUNORESIZERECTANGLECOMMAND_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QPointF>
#include <QSizeF>
#include "C_SebUnoBaseCommand.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SebUnoResizeRectangleCommand :
   public C_SebUnoBaseCommand
{
public:
   C_SebUnoResizeRectangleCommand(QGraphicsScene * const opc_Scene, const std::vector<uint64_t> & orc_Ids,
                                  const QPointF & orc_OldPos, const QSizeF & orc_OldSize, const QPointF & orc_NewPos,
                                  const QSizeF & orc_NewSize, QUndoCommand * const opc_Parent = NULL);
   ~C_SebUnoResizeRectangleCommand() override;
   void undo(void) override;
   void redo(void) override;

private:
   const QPointF mc_OldPos;
   const QSizeF mc_OldSize;
   const QPointF mc_NewPos;
   const QSizeF mc_NewSize;

   void m_UndoSingle(QGraphicsItem * const opc_Item) const;
   void m_RedoSingle(QGraphicsItem * const opc_Item) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
