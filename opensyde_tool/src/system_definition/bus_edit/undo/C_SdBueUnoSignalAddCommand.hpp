//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Signal add undo command (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUEUNOSIGNALADDCOMMAND_H
#define C_SDBUEUNOSIGNALADDCOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SdBueUnoSignalAddDeleteBaseCommand.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueUnoSignalAddCommand :
   public C_SdBueUnoSignalAddDeleteBaseCommand
{
public:
   C_SdBueUnoSignalAddCommand(
      const std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> & orc_MessageId,
      const std::vector<uint32_t> & orc_SignalIndex, const std::vector<uint16_t> & orc_StartBit,
      const std::vector<stw::opensyde_core::C_OscCanSignal::E_MultiplexerType> & orc_MultiplexerType,
      const std::vector<uint16_t> & orc_MultiplexerValue,
      C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
      stw::opensyde_gui::C_SdBueMessageSelectorTreeWidget * const opc_MessageTreeWidget,
      const QString & orc_Text);

   void SetInitialData(const std::vector<stw::opensyde_core::C_OscCanSignal> & orc_Signal,
                       const std::vector<stw::opensyde_core::C_OscNodeDataPoolListElement> & orc_OscSignalCommon,
                       const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UiSignalCommon,
                       const std::vector<C_PuiSdNodeCanSignal> & orc_UiSignal,
                       const std::vector<stw::opensyde_core::C_OscCanProtocol::E_Type> & orc_ProtocolType);

   virtual void redo(void);
   virtual void undo(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
