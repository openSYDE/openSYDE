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
#include "C_SdBueUnoSignalAddDeleteBaseCommand.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueUnoSignalAddCommand :
   public C_SdBueUnoSignalAddDeleteBaseCommand
{
public:
   C_SdBueUnoSignalAddCommand(
      const std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> & orc_MessageId,
      const std::vector<stw_types::uint32> & orc_SignalIndex, const std::vector<stw_types::uint16> & orc_StartBit,
      const std::vector<stw_opensyde_core::C_OSCCanSignal::E_MultiplexerType> & orc_MultiplexerType,
      const std::vector<stw_types::uint16> & orc_MultiplexerValue,
      C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
      stw_opensyde_gui::C_SdBueMessageSelectorTreeWidget * const opc_MessageTreeWidget, const QString & orc_Text);

   void SetInitialData(const std::vector<stw_opensyde_core::C_OSCCanSignal> & orc_Signal,
                       const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement> & orc_OSCSignalCommon,
                       const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UISignalCommon,
                       const std::vector<C_PuiSdNodeCanSignal> & orc_UISignal,
                       const std::vector<stw_opensyde_core::C_OSCCanProtocol::E_Type> & orc_ProtocolType);

   virtual void redo(void);
   virtual void undo(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
