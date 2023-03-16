//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list add undo command (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEUNODATAPOOLLISTADDCOMMAND_H
#define C_SDNDEUNODATAPOOLLISTADDCOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_SdNdeUnoDataPoolListAddDeleteBaseCommand.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeUnoDataPoolListAddCommand :
   public C_SdNdeUnoDataPoolListAddDeleteBaseCommand
{
public:
   C_SdNdeUnoDataPoolListAddCommand(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                    stw::opensyde_gui::C_SdNdeDpListsTreeWidget * const opc_DataPoolListsTreeWidget,
                                    const std::vector<uint32_t> & orc_Indices, QUndoCommand * const opc_Parent = NULL);
   void redo(void) override;
   void undo(void) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
