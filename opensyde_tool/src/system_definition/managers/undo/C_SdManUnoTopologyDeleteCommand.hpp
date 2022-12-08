//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Delete undo command (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDMANUNOTOPOLOGYDELETECOMMAND_H
#define C_SDMANUNOTOPOLOGYDELETECOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_SdManUnoTopologyAddDeleteBaseCommand.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdManUnoTopologyDeleteCommand :
   public C_SdManUnoTopologyAddDeleteBaseCommand
{
public:
   C_SdManUnoTopologyDeleteCommand(QGraphicsScene * const opc_Scene, const std::vector<uint64_t> & orc_Ids,
                                   QUndoCommand * const opc_Parent = NULL);
   virtual ~C_SdManUnoTopologyDeleteCommand(void);
   void undo(void) override;
   void redo(void) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
