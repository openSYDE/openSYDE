//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Reconnect bus connector to new bus undo command (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDMANUNOTOPOLOGYRECONNECTBUSCOMMAND_H
#define C_SDMANUNOTOPOLOGYRECONNECTBUSCOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_SdManUnoTopologyReconnectBaseCommand.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdManUnoTopologyReconnectBusCommand :
   public C_SdManUnoTopologyReconnectBaseCommand
{
public:
   C_SdManUnoTopologyReconnectBusCommand(QGraphicsScene * const opc_Scene,
                                         const std::vector<stw_types::uint64> & orc_IDs,
                                         const stw_types::uint64 & oru64_StartingNodeID,
                                         const stw_types::uint64 & oru64_LastNodeID, const QPointF & orc_ConnectionPos,
                                         const stw_types::sint32 & ors32_Interface,
                                         const std::vector<stw_types::uint8> & orc_NodeIds,
                                         QUndoCommand * const opc_Parent = NULL);
   virtual ~C_SdManUnoTopologyReconnectBusCommand(void);

protected:
   virtual void m_Reconnect(const stw_types::uint64 & oru64_StartingID, const stw_types::uint64 & oru64_LastID,
                            const stw_types::sint32 & ors32_Interface,
                            const std::vector<stw_types::uint8> & orc_NodeIds) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
