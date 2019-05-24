//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Signal delete undo command (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUEUNOSIGNALDELETECOMMAND_H
#define C_SDBUEUNOSIGNALDELETECOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SdBueUnoSignalAddDeleteBaseCommand.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueUnoSignalDeleteCommand :
   public C_SdBueUnoSignalAddDeleteBaseCommand
{
public:
   C_SdBueUnoSignalDeleteCommand(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                 const stw_types::uint32 & oru32_SignalIndex,
                                 C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
                                 stw_opensyde_gui::C_SdBueMessageSelectorTreeWidget * const opc_MessageTreeWidget,
                                 QUndoCommand * const opc_Parent = NULL);

   virtual void redo(void);
   virtual void undo(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
