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

#include "stwtypes.hpp"
#include "C_SdNdeUnoLeDataPoolListElementAddDeleteBaseCommand.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeUnoLeDataPoolListElementMoveCommand :
   public C_SdNdeUnoLeDataPoolListElementAddDeleteBaseCommand
{
public:
   C_SdNdeUnoLeDataPoolListElementMoveCommand(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                              const uint32_t & oru32_DataPoolListIndex,
                                              C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager,
                                              const std::vector<uint32_t> & orc_SourceRow,
                                              const std::vector<uint32_t> & orc_TargetRow,
                                              const bool & orq_AdaptIndices, QUndoCommand * const opc_Parent = NULL);
   void redo(void) override;
   void undo(void) override;

private:
   std::vector<uint32_t> mc_SourceRow;
   std::vector<uint32_t> mc_TargetRow;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
