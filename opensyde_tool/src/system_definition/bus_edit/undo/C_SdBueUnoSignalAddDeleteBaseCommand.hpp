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
#include "C_SdBueUnoMessageBaseCommand.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueUnoSignalAddDeleteBaseCommand :
   public C_SdBueUnoMessageBaseCommand
{
public:
   C_SdBueUnoSignalAddDeleteBaseCommand(
      const std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> & orc_MessageId,
      const std::vector<uint32_t> & orc_SignalIndex, const std::vector<uint16_t> & orc_StartBit,
      const std::vector<stw::opensyde_core::C_OscCanSignal::E_MultiplexerType> & orc_MultiplexerType,
      const std::vector<uint16_t> & orc_MultiplexerValue,
      C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
      stw::opensyde_gui::C_SdBueMessageSelectorTreeWidget * const opc_MessageTreeWidget,
      const QString & orc_Text = "", QUndoCommand * const opc_Parent = NULL);

protected:
   std::vector<uint32_t> mc_SignalIndex;
   std::vector<stw::opensyde_core::C_OscCanSignal> mc_Signal;
   std::vector<stw::opensyde_core::C_OscNodeDataPoolListElement> mc_OscSignalCommon;
   std::vector<C_PuiSdNodeDataPoolListElement> mc_UiSignalCommon;
   std::vector<C_PuiSdNodeCanSignal> mc_UiSignal;

   void m_Add(void);
   void m_Delete(void);

   bool m_CheckSignalsSortedAscending(void) const;

private:
   void m_Store(void);
   void m_Remove(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
