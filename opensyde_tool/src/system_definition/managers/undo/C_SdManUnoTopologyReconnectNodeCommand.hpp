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

#include "C_SdManUnoTopologyReconnectBaseCommand.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdManUnoTopologyReconnectNodeCommand :
   public C_SdManUnoTopologyReconnectBaseCommand
{
public:
   C_SdManUnoTopologyReconnectNodeCommand(QGraphicsScene * const opc_Scene, const std::vector<uint64_t> & orc_Ids,
                                          const uint64_t & oru64_StartingNodeId, const uint64_t & oru64_LastNodeId,
                                          const QPointF & orc_ConnectionPos, const int32_t & ors32_Interface,
                                          const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties,
                                          QUndoCommand * const opc_Parent = NULL);
   ~C_SdManUnoTopologyReconnectNodeCommand(void) override;

protected:
   void m_Reconnect(const uint64_t & oru64_StartingId, const uint64_t & oru64_LastId, const int32_t & ors32_Interface,
                    const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
