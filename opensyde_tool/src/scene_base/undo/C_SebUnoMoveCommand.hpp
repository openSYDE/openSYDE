//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Move undo command (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SEBUNOMOVECOMMAND_HPP
#define C_SEBUNOMOVECOMMAND_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_SebUnoBaseCommand.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SebUnoMoveCommand :
   public C_SebUnoBaseCommand
{
public:
   C_SebUnoMoveCommand(QGraphicsScene * const opc_Scene, const std::vector<uint64_t> & orc_Ids,
                       const QPointF & orc_PositionDifference, QUndoCommand * const opc_Parent = NULL);
   ~C_SebUnoMoveCommand(void) override;
   void undo(void) override;
   void redo(void) override;

   bool mergeWith(const QUndoCommand * const opc_Command) override;

private:
   QPointF mc_PositionDifference;

   void m_UndoSingle(QGraphicsItem * const opc_Item) const;
   void m_RedoSingle(QGraphicsItem * const opc_Item) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
