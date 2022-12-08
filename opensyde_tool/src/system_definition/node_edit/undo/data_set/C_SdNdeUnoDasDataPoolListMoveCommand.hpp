//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list data set move undo command (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEUNODASDATAPOOLLISTMOVECOMMAND_H
#define C_SDNDEUNODASDATAPOOLLISTMOVECOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "stwtypes.hpp"
#include "C_SdNdeUnoDasDataPoolListAddDeleteBaseCommand.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeUnoDasDataPoolListMoveCommand :
   public C_SdNdeUnoDasDataPoolListAddDeleteBaseCommand
{
public:
   C_SdNdeUnoDasDataPoolListMoveCommand(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                        const uint32_t & oru32_DataPoolListIndex,
                                        C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager,
                                        const std::vector<uint32_t> & orc_SourceCol,
                                        const std::vector<uint32_t> & orc_TargetCol, const bool & orq_AdaptIndices,
                                        QUndoCommand * const opc_Parent = NULL);
   void redo(void) override;
   void undo(void) override;

private:
   std::vector<uint32_t> mc_SourceCol;
   std::vector<uint32_t> mc_TargetCol;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
