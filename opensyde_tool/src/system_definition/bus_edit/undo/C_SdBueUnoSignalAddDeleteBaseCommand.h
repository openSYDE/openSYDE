//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Signal base class for add and delete commands (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUEUNOSIGNALADDDELETEBASECOMMAND_H
#define C_SDBUEUNOSIGNALADDDELETEBASECOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SdBueUnoMessageBaseCommand.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueUnoSignalAddDeleteBaseCommand :
   public C_SdBueUnoMessageBaseCommand
{
public:
   C_SdBueUnoSignalAddDeleteBaseCommand(
      const std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> & orc_MessageId,
      const std::vector<stw_types::uint32> & orc_SignalIndex, const std::vector<stw_types::uint16> & orc_StartBit,
      const std::vector<stw_opensyde_core::C_OSCCanSignal::E_MultiplexerType> & orc_MultiplexerType,
      const std::vector<stw_types::uint16> & orc_MultiplexerValue,
      C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
      stw_opensyde_gui::C_SdBueMessageSelectorTreeWidget * const opc_MessageTreeWidget,
      const QString & orc_Text = "", QUndoCommand * const opc_Parent = NULL);

protected:
   std::vector<stw_types::uint32> mc_SignalIndex;
   std::vector<stw_opensyde_core::C_OSCCanSignal> mc_Signal;
   std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement> mc_OSCSignalCommon;
   std::vector<C_PuiSdNodeDataPoolListElement> mc_UISignalCommon;
   std::vector<C_PuiSdNodeCanSignal> mc_UISignal;

   void m_Add(void);
   void m_Delete(void);

   bool m_CheckSignalsSortedAscending(void) const;

private:
   void m_Store(void);
   void m_Remove(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
