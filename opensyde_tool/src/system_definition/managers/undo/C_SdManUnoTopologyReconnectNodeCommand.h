//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Reconnect bus connector to new node undo command (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDMANUNOTOPOLOGYRECONNECTNODECOMMAND_H
#define C_SDMANUNOTOPOLOGYRECONNECTNODECOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_SdManUnoTopologyReconnectBaseCommand.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdManUnoTopologyReconnectNodeCommand :
   public C_SdManUnoTopologyReconnectBaseCommand
{
public:
   C_SdManUnoTopologyReconnectNodeCommand(QGraphicsScene * const opc_Scene,
                                          const std::vector<stw_types::uint64> & orc_IDs,
                                          const stw_types::uint64 & oru64_StartingNodeID,
                                          const stw_types::uint64 & oru64_LastNodeID, const QPointF & orc_ConnectionPos,
                                          const stw_types::sint32 & ors32_Interface,
                                          const stw_types::sint8 & oru8_NodeId, QUndoCommand * const opc_Parent = NULL);
   virtual ~C_SdManUnoTopologyReconnectNodeCommand(void);

protected:
   virtual void m_Reconnect(const stw_types::uint64 & oru64_StartingID, const stw_types::uint64 & oru64_LastID,
                            const stw_types::sint32 & ors32_Interface, const stw_types::uint8 & oru8_NodeId);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
