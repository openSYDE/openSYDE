//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Bus connector reconnect action manager of system definition toplogy (implementation)

   Bus connector reconnect action manager of system definition toplogy

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_SdManTopologyBusConnectorReconnectManager.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdManTopologyBusConnectorReconnectManager::C_SdManTopologyBusConnectorReconnectManager() :
   mpc_BusConnector(NULL),
   mpc_StartingNode(NULL),
   mpc_StartingBus(NULL),
   mpc_LastNode(NULL),
   mpc_LastBus(NULL),
   me_ConnectState(C_GiLiBusConnector::eTO_BUS),
   ms32_NewInterface(-1),
   mc_Properties(),
   mq_ContextMenuActive(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Start connect mode and save all relevant data

   \param[in,out]  opc_BusConnector    Bus connector which has started the reconnect mode
   \param[in]      ore_ConnectState    Conect state of bus connector
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManTopologyBusConnectorReconnectManager::StartReconnectMode(C_GiLiBusConnector * const opc_BusConnector,
                                                                     const C_GiLiBusConnector::E_ConnectState & ore_ConnectState)
{
   this->mpc_BusConnector = opc_BusConnector;
   this->me_ConnectState = ore_ConnectState;
   if (this->mpc_BusConnector != NULL)
   {
      this->mpc_StartingNode = this->mpc_BusConnector->GetNodeItem();
      this->mpc_StartingBus = this->mpc_BusConnector->GetBusItem();
      m_InitLastKnownInteractionPointPosition();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Register waiting for context menu being finished

   \param[in]      orc_ScenePos  Event scene position
   \param[in,out]  opc_Node      Current node
   \param[in]      opc_Bus       Current bus
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManTopologyBusConnectorReconnectManager::EnterWaitForContextMenuState(const QPointF & orc_ScenePos,
                                                                               C_GiNode * const opc_Node,
                                                                               const C_GiLiBus * const opc_Bus)
{
   this->mc_ConnectionPos = orc_ScenePos;
   this->mpc_LastNode = opc_Node;
   this->mpc_LastBus = opc_Bus;
   this->mq_ContextMenuActive = true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for context menu being closed soon
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManTopologyBusConnectorReconnectManager::ContextMenuAboutToClose(void)
{
   this->mq_ContextMenuActive = false;
   m_FinishReconnect();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Register interface accepted

   \param[in]  ors32_Interface   Selected interface number
   \param[in]  orc_Properties    Properties
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManTopologyBusConnectorReconnectManager::ContextMenuAccepted(const sint32 & ors32_Interface,
                                                                      const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties)
{
   this->ms32_NewInterface = ors32_Interface;
   this->mc_Properties = orc_Properties;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Deactivate reconnection
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManTopologyBusConnectorReconnectManager::DeactivateReconnection(void)
{
   //Delay if timer active
   if (this->mq_ContextMenuActive == false)
   {
      //Reset all
      this->mpc_BusConnector = NULL;
      this->mpc_StartingNode = NULL;
      this->mpc_StartingBus = NULL;
      this->mpc_LastNode = NULL;
      this->mpc_LastBus = NULL;
      this->ms32_NewInterface = -1;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Is reconnect manager expected to handle this event

   \return
   true:  Handling done by reconnect manager
   false: No handling expected
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdManTopologyBusConnectorReconnectManager::Active(void) const
{
   return this->mq_ContextMenuActive;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Final step of reconnection when all events should have occured

   This function expects to have all information necessary to either fully do or undo the reconnection step
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManTopologyBusConnectorReconnectManager::m_FinishReconnect(void)
{
   if (this->ms32_NewInterface >= 0)
   {
      //Do
      if (this->me_ConnectState == C_GiLiBusConnector::E_ConnectState::eTO_BUS)
      {
         Q_EMIT this->SigReconnectBus(this->mpc_BusConnector, this->mpc_StartingBus, this->mpc_LastBus,
                                      this->mc_ConnectionPos,
                                      this->ms32_NewInterface, this->mc_Properties);
      }
      else
      {
         Q_EMIT this->SigReconnectNode(this->mpc_BusConnector, this->mpc_StartingNode, this->mpc_LastNode,
                                       this->mc_ConnectionPos, this->ms32_NewInterface, this->mc_Properties);
      }
   }
   else
   {
      //Revert
      if (this->me_ConnectState == C_GiLiBusConnector::E_ConnectState::eTO_BUS)
      {
         Q_EMIT this->SigRevertBus(this->mpc_BusConnector, this->mpc_StartingBus, this->mpc_LastBus,
                                   this->mc_LastKnownInteractionPointPosition);
      }
      else
      {
         Q_EMIT this->SigRevertNode(this->mpc_BusConnector, this->mpc_StartingNode, this->mpc_LastNode,
                                    this->mc_LastKnownInteractionPointPosition);
      }
   }
   DeactivateReconnection();
   //Last step
   Q_EMIT this->SigCleanUpPorts();
   Q_EMIT this->SigCleanUpTemporaryLine();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize last known interaction point position with current value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManTopologyBusConnectorReconnectManager::m_InitLastKnownInteractionPointPosition(void)
{
   if (this->mpc_BusConnector != NULL)
   {
      if (this->me_ConnectState == C_GiLiBusConnector::E_ConnectState::eTO_BUS)
      {
         this->mpc_BusConnector->GetBusInteractionScenePos(this->mc_LastKnownInteractionPointPosition);
      }
      else
      {
         this->mpc_BusConnector->GetNodeInteractionScenePos(this->mc_LastKnownInteractionPointPosition);
      }
   }
}
