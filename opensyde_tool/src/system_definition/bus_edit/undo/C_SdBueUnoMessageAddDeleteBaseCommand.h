//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Message base class for add and delete commands (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     20.04.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDBUEUNOMESSAGEADDDELETEBASECOMMAND_H
#define C_SDBUEUNOMESSAGEADDDELETEBASECOMMAND_H

/* -- Includes ------------------------------------------------------------- */
#include "C_SdBueUnoMessageBaseCommand.h"
#include "C_OSCCanMessage.h"
#include "C_OSCNodeDataPoolListElement.h"
#include "C_PuiSdNodeDataPoolListElement.h"
#include "C_PuiSdNodeCanSignal.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SdBueUnoMessageAddDeleteBaseCommand :
   public C_SdBueUnoMessageBaseCommand
{
public:
   C_SdBueUnoMessageAddDeleteBaseCommand(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                         C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
                                         stw_opensyde_gui::C_SdBueMessageSelectorTreeWidget * const opc_MessageTreeWidget, const QString & orc_Text = "",
                                         QUndoCommand * const opc_Parent = NULL);

protected:
   //Avoid using the message index under all circumstances!!!!!
   stw_opensyde_core::C_OSCCanMessageIdentificationIndices mc_LastMessageId;
   stw_opensyde_core::C_OSCCanMessage mc_Message;
   std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement> mc_OSCSignalCommons;
   std::vector<C_PuiSdNodeDataPoolListElement> mc_UISignalCommons;
   std::vector<C_PuiSdNodeCanSignal> mc_UISignals;
   std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> mc_MatchingIds;

   void Add(void);
   void Delete(void);

private:
   void m_Store(void);
   void m_Remove(void);

   static stw_types::sint32 mh_CheckSync(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
