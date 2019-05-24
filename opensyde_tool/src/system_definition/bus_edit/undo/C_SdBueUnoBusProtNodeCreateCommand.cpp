//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Create data pool undo command (implementation)

   Create data pool undo command

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_SdBueUnoBusProtNodeCreateCommand.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;

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
C_SdBueUnoBusProtNodeCreateCommand::C_SdBueUnoBusProtNodeCreateCommand(const uint32 ou32_NodeIndex,
                                                                       const uint32 ou32_InterfaceIndex,
                                                                       const C_OSCCanProtocol::E_Type oe_Protocol,
                                                                       QWidget * const opc_Widget,
                                                                       QUndoCommand * const opc_Parent) :
   C_SdBueUnoBusProtNodeConnectDisconnectBaseCommand(ou32_NodeIndex, ou32_InterfaceIndex, oe_Protocol, opc_Widget,
                                                     "Create data pool", opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Redo connect to protocol and create data pool
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoBusProtNodeCreateCommand::redo(void)
{
   this->m_DoAddDataPool();
   C_SdBueUnoBusProtNodeConnectDisconnectBaseCommand::redo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Undo connect to protocol and create data pool
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoBusProtNodeCreateCommand::undo(void)
{
   C_SdBueUnoBusProtNodeConnectDisconnectBaseCommand::undo();
   this->m_DoDeleteDataPool();
}
