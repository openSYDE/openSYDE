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
   C_SdBueUnoSignalAddDeleteBaseCommand(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                        const stw_types::uint32 & oru32_SignalIndex,
                                        const stw_types::uint16 ou16_StartBit,
                                        C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
                                        stw_opensyde_gui::C_SdBueMessageSelectorTreeWidget * const opc_MessageTreeWidget, const QString & orc_Text = "",
                                        QUndoCommand * const opc_Parent = NULL);

protected:
   stw_types::uint32 mu32_SignalIndex;
   stw_opensyde_core::C_OSCCanSignal mc_Signal;
   stw_opensyde_core::C_OSCNodeDataPoolListElement mc_OSCSignalCommon;
   C_PuiSdNodeDataPoolListElement mc_UISignalCommon;
   C_PuiSdNodeCanSignal mc_UISignal;

   void Add(void);
   void Delete(void);

private:
   void m_Store(void);
   void m_Remove(void) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
