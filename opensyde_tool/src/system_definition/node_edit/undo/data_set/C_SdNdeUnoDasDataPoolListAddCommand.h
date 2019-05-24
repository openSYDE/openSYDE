//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list data set add undo command (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEUNODASDATAPOOLLISTADDCOMMAND_H
#define C_SDNDEUNODASDATAPOOLLISTADDCOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_SdNdeUnoDasDataPoolListAddDeleteBaseCommand.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeUnoDasDataPoolListAddCommand :
   public C_SdNdeUnoDasDataPoolListAddDeleteBaseCommand
{
public:
   C_SdNdeUnoDasDataPoolListAddCommand(const stw_types::uint32 & oru32_NodeIndex,
                                       const stw_types::uint32 & oru32_DataPoolIndex,
                                       const stw_types::uint32 & oru32_DataPoolListIndex,
                                       C_SdNdeDataPoolListModelViewManager * const opc_DataPoolListModelViewManager,
                                       const std::vector<stw_types::uint32> & orc_Indices,
                                       QUndoCommand * const opc_Parent = NULL);
   virtual void redo(void) override;
   virtual void undo(void) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
