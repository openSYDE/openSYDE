//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list data set add specific undo command (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEUNODASDATAPOOLLISTADDSPECIFICCOMMAND_H
#define C_SDNDEUNODASDATAPOOLLISTADDSPECIFICCOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_SdNdeUnoDasDataPoolListAddCommand.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeUnoDasDataPoolListAddSpecificCommand :
   public C_SdNdeUnoDasDataPoolListAddCommand
{
public:
   C_SdNdeUnoDasDataPoolListAddSpecificCommand(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                               const uint32_t & oru32_DataPoolListIndex,
                                               C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager,
                                               const std::vector<uint32_t> & orc_Indices,
                                               const std::vector<stw::opensyde_core::C_OscNodeDataPoolDataSet> & orc_OscNames,
                                               const std::vector<std::vector<stw::opensyde_core::C_OscNodeDataPoolContent> > & orc_OscDataSetValues,
                                               QUndoCommand * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
