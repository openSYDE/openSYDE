//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Connect node to protcol and create data pool undo command (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUEUNOBUSPROTNODECONNECTANDCREATECOMMAND_H
#define C_SDBUEUNOBUSPROTNODECONNECTANDCREATECOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SdBueUnoBusProtNodeConnectDisconnectBaseCommand.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueUnoBusProtNodeConnectAndCreateCommand :
   public C_SdBueUnoBusProtNodeConnectDisconnectBaseCommand
{
public:
   C_SdBueUnoBusProtNodeConnectAndCreateCommand(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex,
                                                const stw::opensyde_core::C_OscCanProtocol::E_Type oe_Protocol,
                                                QWidget * const opc_Widget, QUndoCommand * const opc_Parent = NULL);

   void redo(void) override;
   void undo(void) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
