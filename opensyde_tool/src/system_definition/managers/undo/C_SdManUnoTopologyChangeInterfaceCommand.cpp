//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       short description (implementation)

   detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     12.12.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_SdManUnoTopologyChangeInterfaceCommand.h"
#include "C_GiLiBusConnector.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace std;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \param[in,out] opc_Scene              Pointer to currently active scene
   \param[in]     orc_IDs                Affected unique IDs
   \param[in]     oru8_PreviousInterface Previous interface number
   \param[in]     oru8_NewInterface      New interface number to use
   \param[in]     oru8_PreviousNodeId    Previous node id
   \param[in]     oru8_NewNodeId         New node id
   \param[in,out] opc_Parent             Optional pointer to parent

   \created     12.12.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdManUnoTopologyChangeInterfaceCommand::C_SdManUnoTopologyChangeInterfaceCommand(QGraphicsScene * const opc_Scene,
                                                                                   const std::vector<uint64> & orc_IDs,
                                                                                   const uint8 & oru8_PreviousInterface,
                                                                                   const uint8 & oru8_NewInterface,
                                                                                   const uint8 & oru8_PreviousNodeId,
                                                                                   const uint8 & oru8_NewNodeId,
                                                                                   QUndoCommand * const opc_Parent) :
   C_SebUnoBaseCommand(opc_Scene, orc_IDs, "Change interface of bus connection(s)", opc_Parent),
   mu8_PreviousInterface(oru8_PreviousInterface),
   mu8_NewInterface(oru8_NewInterface),
   mu8_PreviousNodeId(oru8_PreviousNodeId),
   mu8_NewNodeId(oru8_NewNodeId)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     12.12.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdManUnoTopologyChangeInterfaceCommand::~C_SdManUnoTopologyChangeInterfaceCommand(void)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Undo change interface

   \created     12.12.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdManUnoTopologyChangeInterfaceCommand::undo(void)
{
   m_ChangeInterface(this->mu8_PreviousInterface, this->mu8_PreviousNodeId);
   QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Redo change interface

   \created     12.12.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdManUnoTopologyChangeInterfaceCommand::redo(void)
{
   m_ChangeInterface(this->mu8_NewInterface, this->mu8_NewNodeId);
   QUndoCommand::redo();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Change interface of all bus connectors to specified one

   \param[in] oru8_NewInterface New interface to change to
   \param[in] oru8_NodeId       New node id

   \created     12.12.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdManUnoTopologyChangeInterfaceCommand::m_ChangeInterface(const uint8 & oru8_NewInterface,
                                                                 const uint8 & oru8_NodeId)
{
   vector<QGraphicsItem *> c_Items = m_GetSceneItems();
   for (vector<QGraphicsItem *>::const_iterator c_ItItem = c_Items.begin(); c_ItItem != c_Items.end(); ++c_ItItem)
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_GiLiBusConnector * const pc_CurConn = dynamic_cast<C_GiLiBusConnector *>(*c_ItItem);
      if (pc_CurConn != NULL)
      {
         pc_CurConn->ChangeInterface(oru8_NewInterface, oru8_NodeId);
      }
   }
}
