//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list element delete undo command (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEUNOLEDATAPOOLLISTELEMENTDELETECOMMAND_HPP
#define C_SDNDEUNOLEDATAPOOLLISTELEMENTDELETECOMMAND_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_SdNdeUnoLeDataPoolListElementAddDeleteBaseCommand.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeUnoLeDataPoolListElementDeleteCommand :
   public C_SdNdeUnoLeDataPoolListElementAddDeleteBaseCommand
{
public:
   C_SdNdeUnoLeDataPoolListElementDeleteCommand(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                                const uint32_t & oru32_DataPoolListIndex,
                                                C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager,
                                                const std::vector<uint32_t> & orc_Indices,
                                                QUndoCommand * const opc_Parent = NULL);
   void redo(void) override;
   void undo(void) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
