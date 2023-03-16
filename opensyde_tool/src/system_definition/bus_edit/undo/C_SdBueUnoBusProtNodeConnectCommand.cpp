//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Connect node to protocol undo command (implementation)

   Connect node to protocol undo command

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_SdBueUnoBusProtNodeConnectCommand.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in]     ou32_NodeIndex      Node index
   \param[in]     ou32_InterfaceIndex Interface index
   \param[in]     oe_Protocol         Com protocol
   \param[in,out] opc_Widget          Widget to notify for changes
   \param[in,out] opc_Parent          Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueUnoBusProtNodeConnectCommand::C_SdBueUnoBusProtNodeConnectCommand(const uint32_t ou32_NodeIndex,
                                                                         const uint32_t ou32_InterfaceIndex,
                                                                         const C_OscCanProtocol::E_Type oe_Protocol,
                                                                         QWidget * const opc_Widget,
                                                                         QUndoCommand * const opc_Parent) :
   C_SdBueUnoBusProtNodeConnectDisconnectBaseCommand(ou32_NodeIndex, ou32_InterfaceIndex, oe_Protocol, opc_Widget,
                                                     "Connect node to protocol", opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Redo connect to protocol
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoBusProtNodeConnectCommand::redo(void)
{
   this->m_DoConnectNodeToProt();
   C_SdBueUnoBusProtNodeConnectDisconnectBaseCommand::redo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Undo connect to protocol
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoBusProtNodeConnectCommand::undo(void)
{
   C_SdBueUnoBusProtNodeConnectDisconnectBaseCommand::undo();
   this->m_DoDisconnectNodeFromProt();
}
