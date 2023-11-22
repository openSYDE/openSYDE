//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Move / change bus connector undo command (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SEBUNOTOPBUSCONNECTORMOVECOMMAND_HPP
#define C_SEBUNOTOPBUSCONNECTORMOVECOMMAND_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QPointF>
#include "C_SebUnoBaseCommand.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SebUnoTopBusConnectorMoveCommand :
   public C_SebUnoBaseCommand
{
public:
   C_SebUnoTopBusConnectorMoveCommand(QGraphicsScene * const opc_Scene, const std::vector<uint64_t> & orc_Ids,
                                      const std::vector<QPointF> & orc_InitialPoints,
                                      const std::vector<QPointF> & orc_FinalPoints,
                                      QUndoCommand * const opc_Parent = NULL);
   C_SebUnoTopBusConnectorMoveCommand(const C_SebUnoTopBusConnectorMoveCommand * const opc_Prev = NULL,
                                      QUndoCommand * const opc_Parent = NULL);
   ~C_SebUnoTopBusConnectorMoveCommand(void) override;
   void undo(void) override;
   void redo(void) override;

private:
   const std::vector<QPointF> mc_Initial;
   const std::vector<QPointF> mc_Final;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
