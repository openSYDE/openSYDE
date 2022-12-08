//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Remove bend point for line undo command (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SEBUNOTOPREMOVEBENDPOINTCOMMAND_H
#define C_SEBUNOTOPREMOVEBENDPOINTCOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_SebUnoBendPointBaseCommand.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SebUnoRemoveBendPointCommand :
   public C_SebUnoBendPointBaseCommand
{
public:
   C_SebUnoRemoveBendPointCommand(QGraphicsScene * const opc_Scene, const std::vector<uint64_t> & orc_Ids,
                                  const QPointF & orc_ScenePosition, QUndoCommand * const opc_Parent = NULL);
   virtual ~C_SebUnoRemoveBendPointCommand(void);
   void undo(void) override;
   void redo(void) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
