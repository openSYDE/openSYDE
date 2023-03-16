//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list element add specific undo command (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEUNOLEDATAPOOLLISTELEMENTADDSPECIFICCOMMAND_H
#define C_SDNDEUNOLEDATAPOOLLISTELEMENTADDSPECIFICCOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_SdNdeUnoLeDataPoolListElementAddCommand.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeUnoLeDataPoolListElementAddSpecificCommand :
   public C_SdNdeUnoLeDataPoolListElementAddCommand
{
public:
   C_SdNdeUnoLeDataPoolListElementAddSpecificCommand(const uint32_t & oru32_NodeIndex,
                                                     const uint32_t & oru32_DataPoolIndex,
                                                     const uint32_t & oru32_DataPoolListIndex,
                                                     C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager,
                                                     const std::vector<uint32_t> & orc_Indices,
                                                     const std::vector<stw::opensyde_core::C_OscNodeDataPoolListElement> & orc_OscData,
                                                     const std::vector<stw::opensyde_gui_logic::
                                                                       C_PuiSdNodeDataPoolListElement> & orc_UiData,
                                                     QUndoCommand * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
