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
   C_SdBueUnoSignalAddCommand(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                              const stw_types::uint32 & oru32_SignalIndex, const stw_types::uint16 ou16_StartBit,
                              const stw_opensyde_core::C_OSCCanSignal::E_MultiplexerType oe_MultiplexerType,
                              const stw_types::uint16 ou16_MultiplexerValue,
                              C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
                              stw_opensyde_gui::C_SdBueMessageSelectorTreeWidget * const opc_MessageTreeWidget,
                              QUndoCommand * const opc_Parent = NULL);

   void SetInitialData(const stw_opensyde_core::C_OSCCanSignal & orc_Signal,
                       const stw_opensyde_core::C_OSCNodeDataPoolListElement & orc_OSCSignalCommon,
                       const C_PuiSdNodeDataPoolListElement & orc_UISignalCommon,
                       const C_PuiSdNodeCanSignal & orc_UISignal,
                       const stw_opensyde_core::C_OSCCanProtocol::E_Type oe_ProtocolType);

   virtual void redo(void);
   virtual void undo(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
