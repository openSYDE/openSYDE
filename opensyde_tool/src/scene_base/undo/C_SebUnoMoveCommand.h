//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Move undo command (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SEBUNOMOVECOMMAND_H
#define C_SEBUNOMOVECOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_SebUnoBaseCommand.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SebUnoMoveCommand :
   public C_SebUnoBaseCommand
{
public:
   C_SebUnoMoveCommand(QGraphicsScene * const opc_Scene, const std::vector<stw_types::uint64> & orc_IDs,
                       const QPointF & orc_PositionDifference, QUndoCommand * const opc_Parent = NULL);
   virtual ~C_SebUnoMoveCommand(void);
   virtual void undo(void) override;
   virtual void redo(void) override;

   virtual bool mergeWith(const QUndoCommand * const opc_Command);

private:
   QPointF mc_PositionDifference;

   void m_UndoSingle(QGraphicsItem * const opc_Item) const;
   void m_RedoSingle(QGraphicsItem * const opc_Item) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
