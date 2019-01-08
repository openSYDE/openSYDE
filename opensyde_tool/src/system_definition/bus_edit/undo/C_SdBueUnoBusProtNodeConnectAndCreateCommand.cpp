//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Connect node to protcol and create data pool undo command (implementation)

   Connect node to protcol and create data pool undo command

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     21.04.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_SdBueUnoBusProtNodeConnectAndCreateCommand.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \param[in]     ou32_NodeIndex      Node index
   \param[in]     ou32_InterfaceIndex Interface index
   \param[in]     oe_Protocol         Com protocol
   \param[in,out] opc_Widget          Widget to notify for changes
   \param[in,out] opc_Parent          Optional pointer to parent

   \created     21.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdBueUnoBusProtNodeConnectAndCreateCommand::C_SdBueUnoBusProtNodeConnectAndCreateCommand(const uint32 ou32_NodeIndex,
                                                                                           const uint32 ou32_InterfaceIndex, const C_OSCCanProtocol::E_Type oe_Protocol, QWidget * const opc_Widget,
                                                                                           QUndoCommand * const opc_Parent)
   :
   C_SdBueUnoBusProtNodeConnectDisconnectBaseCommand(ou32_NodeIndex, ou32_InterfaceIndex, oe_Protocol, opc_Widget,
                                                     "Connect node to protocol and create Datapool", opc_Parent)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Redo connect to protocol and create data pool

   \created     21.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueUnoBusProtNodeConnectAndCreateCommand::redo(void)
{
   this->m_DoConnectNodeToProtAndAddDataPool();
   C_SdBueUnoBusProtNodeConnectDisconnectBaseCommand::redo();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Undo connect to protocol and create data pool

   \created     21.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueUnoBusProtNodeConnectAndCreateCommand::undo(void)
{
   C_SdBueUnoBusProtNodeConnectDisconnectBaseCommand::undo();
   this->m_DoDisconnectNodeToProtAndDeleteDataPool();
}
