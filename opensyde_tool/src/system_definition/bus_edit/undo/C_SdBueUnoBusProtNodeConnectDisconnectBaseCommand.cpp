//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Bus protocol for specific node base class for connect and disconnect commands (implementation)

   detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwerrors.hpp"

#include "C_SdBueUnoBusProtNodeConnectDisconnectBaseCommand.hpp"

#include "C_PuiSdHandler.hpp"
#include "C_OscNode.hpp"
#include "C_OscNodeDataPool.hpp"
#include "C_SdBueComIfDescriptionWidget.hpp"
#include "C_GtGetText.hpp"
#include "C_PuiSdUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui;
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
   \param[in]     orc_Text            Optional command text for informational display
   \param[in,out] opc_Parent          Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueUnoBusProtNodeConnectDisconnectBaseCommand::C_SdBueUnoBusProtNodeConnectDisconnectBaseCommand(
   const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex, const C_OscCanProtocol::E_Type oe_Protocol,
   QWidget * const opc_Widget, const QString & orc_Text, QUndoCommand * const opc_Parent) :
   QUndoCommand(orc_Text, opc_Parent),
   mu32_NodeIndex(ou32_NodeIndex),
   mu32_InterfaceIndex(ou32_InterfaceIndex),
   me_Protocol(oe_Protocol),
   mpc_Widget(opc_Widget)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Connect node & interface to protocol
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoBusProtNodeConnectDisconnectBaseCommand::m_DoConnectNodeToProt(void)
{
   this->m_SetComProtocolUsedByInterfaceFlag(true);
   m_UpdateUi();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Connect node & interface to protocol and create data pool
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoBusProtNodeConnectDisconnectBaseCommand::m_DoConnectNodeToProtAndAddDataPool(void)
{
   if (this->m_AddDataPool() == C_NO_ERR)
   {
      this->m_SetComProtocolUsedByInterfaceFlag(true);
   }
   m_UpdateUi();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Disconnect node & interface from protocol
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoBusProtNodeConnectDisconnectBaseCommand::m_DoDisconnectNodeFromProt(void)
{
   this->m_SetComProtocolUsedByInterfaceFlag(false);
   m_UpdateUi();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Disconnect node & interface from protocol and delete data pool
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoBusProtNodeConnectDisconnectBaseCommand::m_DoDisconnectNodeToProtAndDeleteDataPool(void)
{
   this->m_SetComProtocolUsedByInterfaceFlag(false);

   //Delete data pool
   this->m_DeleteDataPool();

   m_UpdateUi();
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoBusProtNodeConnectDisconnectBaseCommand::m_SetComProtocolUsedByInterfaceFlag(const bool oq_Flag) const
{
   C_PuiSdHandler::h_GetInstance()->SetCanProtocolMessageContainerConnected(this->mu32_NodeIndex,
                                                                            this->me_Protocol,
                                                                            this->mu32_InterfaceIndex, oq_Flag);
}

//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdBueUnoBusProtNodeConnectDisconnectBaseCommand::m_AddDataPool(void) const
{
   return C_PuiSdHandler::h_GetInstance()->AddAutoGenCommDataPool(this->mu32_NodeIndex, this->me_Protocol);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoBusProtNodeConnectDisconnectBaseCommand::m_DeleteDataPool(void) const
{
   const C_OscNode * pc_Node;

   //Delete data pool
   pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);
   if (pc_Node != NULL)
   {
      uint32_t u32_ItComDataPool = 0;
      bool q_Found = false;
      //Match com data pool to com protocol type
      for (uint32_t u32_ItDataPool = 0; (u32_ItDataPool < pc_Node->c_DataPools.size()) && (q_Found == false);
           ++u32_ItDataPool)
      {
         const C_OscNodeDataPool & orc_DataPool = pc_Node->c_DataPools[u32_ItDataPool];
         if (orc_DataPool.e_Type == C_OscNodeDataPool::eCOM)
         {
            if (u32_ItComDataPool < pc_Node->c_ComProtocols.size())
            {
               const C_OscCanProtocol & orc_Protocol = pc_Node->c_ComProtocols[u32_ItComDataPool];
               if (orc_Protocol.e_Type == this->me_Protocol)
               {
                  //Everything matches
                  C_PuiSdHandler::h_GetInstance()->RemoveDataPool(this->mu32_NodeIndex, u32_ItDataPool);
                  q_Found = true;
               }
            }
            ++u32_ItComDataPool;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Notify ui
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoBusProtNodeConnectDisconnectBaseCommand::m_UpdateUi(void)
{
   C_SdBueComIfDescriptionWidget * const pc_Widget =
      dynamic_cast<C_SdBueComIfDescriptionWidget * const>(this->mpc_Widget);

   if (pc_Widget != NULL)
   {
      pc_Widget->PartialReload();
   }
}
