//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Message add undo command (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUEUNOMESSAGEADDCOMMAND_H
#define C_SDBUEUNOMESSAGEADDCOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SdBueUnoMessageAddDeleteBaseCommand.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueUnoMessageAddCommand :
   public C_SdBueUnoMessageAddDeleteBaseCommand
{
public:
   C_SdBueUnoMessageAddCommand(
      const std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> & orc_MessageId,
      C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
      stw_opensyde_gui::C_SdBueMessageSelectorTreeWidget * const opc_MessageTreeWidget, const QString & orc_Text);

   void SetInitialData(const std::vector<stw_opensyde_core::C_OSCCanMessage > & orc_Message,
                       const std::vector<std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement> > & orc_OSCSignalCommons, const std::vector<std::vector<C_PuiSdNodeDataPoolListElement> > & orc_UISignalCommons, const std::vector<std::vector<C_PuiSdNodeCanSignal> > & orc_UISignals, const std::vector<std::vector<QString> > & orc_OwnerNodeName, const std::vector<std::vector<stw_types::uint32> > & orc_OwnerNodeInterfaceIndex, const std::vector<std::vector<stw_types::uint32> > & orc_OwnerNodeDatapoolIndex, const std::vector<std::vector<bool> > & orc_OwnerIsTxFlag);

   std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> GetLastMessageIds(void) const;

   virtual void redo(void);
   virtual void undo(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
