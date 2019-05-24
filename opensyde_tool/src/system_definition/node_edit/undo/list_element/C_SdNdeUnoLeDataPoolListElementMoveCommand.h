//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list element move undo command (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEUNOLEDATAPOOLLISTELEMENTMOVECOMMAND_H
#define C_SDNDEUNOLEDATAPOOLLISTELEMENTMOVECOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "stwtypes.h"
#include "C_SdNdeUnoLeDataPoolListElementAddDeleteBaseCommand.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeUnoLeDataPoolListElementMoveCommand :
   public C_SdNdeUnoLeDataPoolListElementAddDeleteBaseCommand
{
public:
   C_SdNdeUnoLeDataPoolListElementMoveCommand(const stw_types::uint32 & oru32_NodeIndex,
                                              const stw_types::uint32 & oru32_DataPoolIndex,
                                              const stw_types::uint32 & oru32_DataPoolListIndex,
                                              C_SdNdeDataPoolListModelViewManager * const opc_DataPoolListModelViewManager,
                                              const std::vector<stw_types::uint32> & orc_SourceRow,
                                              const std::vector<stw_types::uint32> & orc_TargetRow, const bool & orq_AdaptIndices,
                                              QUndoCommand * const opc_Parent = NULL);
   virtual void redo(void) override;
   virtual void undo(void) override;

private:
   std::vector<stw_types::uint32> mc_SourceRow;
   std::vector<stw_types::uint32> mc_TargetRow;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
