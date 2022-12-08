//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Add bend point for line undo command (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SEBUNOTOPADDBENDPOINTCOMMAND_H
#define C_SEBUNOTOPADDBENDPOINTCOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_SebUnoBendPointBaseCommand.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SebUnoAddBendPointCommand :
   public C_SebUnoBendPointBaseCommand
{
public:
   C_SebUnoAddBendPointCommand(QGraphicsScene * const opc_Scene, const std::vector<uint64_t> & orc_Ids,
                               const QPointF & orc_ScenePosition, QUndoCommand * const opc_Parent = NULL);
   virtual ~C_SebUnoAddBendPointCommand(void);
   void undo(void) override;
   void redo(void) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
