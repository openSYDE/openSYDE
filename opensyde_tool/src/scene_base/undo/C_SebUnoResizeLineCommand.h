//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Resize line undo command (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SEBUNOTOPRESIZELINECOMMAND_H
#define C_SEBUNOTOPRESIZELINECOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_SebUnoBaseCommand.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SebUnoTopResizeLineCommand :
   public C_SebUnoBaseCommand
{
public:
   C_SebUnoTopResizeLineCommand(QGraphicsScene * const opc_Scene, const std::vector<stw_types::uint64> & orc_IDs,
                                const stw_types::sint32 & ors32_InteractionPointID,
                                const QPointF & orc_PositionDifference, QUndoCommand * const opc_Parent = NULL);
   virtual ~C_SebUnoTopResizeLineCommand();
   virtual void undo(void) override;
   virtual void redo(void) override;

private:
   const stw_types::sint32 ms32_InteractionPointID;
   const QPointF mc_PositionDifference;

   void m_UndoSingle(QGraphicsItem * const opc_Item) const;
   void m_RedoSingle(QGraphicsItem * const opc_Item) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
