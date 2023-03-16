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
#include "C_SdBueUnoMessageAddDeleteBaseCommand.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueUnoMessageAddCommand :
   public C_SdBueUnoMessageAddDeleteBaseCommand
{
public:
   C_SdBueUnoMessageAddCommand(
      const std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> & orc_MessageId,
      C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
      stw::opensyde_gui::C_SdBueMessageSelectorTreeWidget * const opc_MessageTreeWidget,
      const QString & orc_Text);

   void SetInitialData(const std::vector<stw::opensyde_core::C_OscCanMessage > & orc_Message,
                       const std::vector<std::vector<stw::opensyde_core::C_OscNodeDataPoolListElement> > & orc_OscSignalCommons, const std::vector<std::vector<C_PuiSdNodeDataPoolListElement> > & orc_UiSignalCommons, const std::vector<std::vector<C_PuiSdNodeCanSignal> > & orc_UiSignals, const std::vector<std::vector<QString> > & orc_OwnerNodeName, const std::vector<std::vector<uint32_t> > & orc_OwnerNodeInterfaceIndex, const std::vector<std::vector<uint32_t> > & orc_OwnerNodeDatapoolIndex, const std::vector<std::vector<bool> > & orc_OwnerIsTxFlag);

   std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> GetLastMessageIds(void) const;

   void redo(void) override;
   void undo(void) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
