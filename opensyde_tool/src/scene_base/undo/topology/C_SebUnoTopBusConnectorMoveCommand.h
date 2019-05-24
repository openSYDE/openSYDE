//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Move / change bus connector undo command (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SEBUNOTOPBUSCONNECTORMOVECOMMAND_H
#define C_SEBUNOTOPBUSCONNECTORMOVECOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QPointF>
#include "C_SebUnoBaseCommand.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SebUnoTopBusConnectorMoveCommand :
   public C_SebUnoBaseCommand
{
public:
   C_SebUnoTopBusConnectorMoveCommand(QGraphicsScene * const opc_Scene, const std::vector<stw_types::uint64> & orc_IDs,
                                      const std::vector<QPointF> & orc_InitialPoints,
                                      const std::vector<QPointF> & orc_FinalPoints,
                                      QUndoCommand * const opc_Parent = NULL);
   C_SebUnoTopBusConnectorMoveCommand(const C_SebUnoTopBusConnectorMoveCommand * const opc_Prev,
                                      QUndoCommand * const opc_Parent = NULL);
   virtual ~C_SebUnoTopBusConnectorMoveCommand(void);
   virtual void undo(void) override;
   virtual void redo(void) override;

private:
   const std::vector<QPointF> mc_Initial;
   const std::vector<QPointF> mc_Final;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
