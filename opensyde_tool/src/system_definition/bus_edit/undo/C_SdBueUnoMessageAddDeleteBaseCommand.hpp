//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Message base class for add and delete commands (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUEUNOMESSAGEADDDELETEBASECOMMAND_H
#define C_SDBUEUNOMESSAGEADDDELETEBASECOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SdBueUnoMessageBaseCommand.hpp"
#include "C_OscCanMessage.hpp"
#include "C_OscNodeDataPoolListElement.hpp"
#include "C_PuiSdNodeDataPoolListElement.hpp"
#include "C_PuiSdNodeCanSignal.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueUnoMessageAddDeleteBaseCommand :
   public C_SdBueUnoMessageBaseCommand
{
public:
   C_SdBueUnoMessageAddDeleteBaseCommand(
      const std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> & orc_MessageId,
      C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager, stw::opensyde_gui::
      C_SdBueMessageSelectorTreeWidget * const opc_MessageTreeWidget,
      const QString & orc_Text = "", QUndoCommand * const opc_Parent = NULL);

protected:
   //Avoid using the message index under all circumstances!!!!!
   std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> mc_LastMessageId;
   std::vector<stw::opensyde_core::C_OscCanMessage> mc_Message;
   std::vector<std::vector<stw::opensyde_core::C_OscNodeDataPoolListElement> > mc_OscSignalCommons;
   std::vector<std::vector<C_PuiSdNodeDataPoolListElement> > mc_UiSignalCommons;
   std::vector<std::vector<C_PuiSdNodeCanSignal> > mc_UiSignals;
   std::vector<std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> > mc_MatchingIds;

   void m_Add(void);
   void m_Delete(void);

private:
   void m_Store(void);
   void m_Remove(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
