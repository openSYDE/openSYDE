//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Message delete undo command (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUEUNOMESSAGEDELETECOMMAND_H
#define C_SDBUEUNOMESSAGEDELETECOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SdBueUnoMessageAddDeleteBaseCommand.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueUnoMessageDeleteCommand :
   public C_SdBueUnoMessageAddDeleteBaseCommand
{
public:
   C_SdBueUnoMessageDeleteCommand(
      const std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> & orc_MessageId,
      C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
      stw::opensyde_gui::C_SdBueMessageSelectorTreeWidget * const opc_MessageTreeWidget,
      QUndoCommand * const opc_Parent = NULL);

   void redo(void) override;
   void undo(void) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
