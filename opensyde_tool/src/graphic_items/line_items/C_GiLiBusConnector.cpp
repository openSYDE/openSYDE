//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Connection of node to bus (implementation)

   Class connects a node with a bus on the gui. It calculates the necessary coordinates
   if one item is moved and keeps the connection stable

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QGraphicsScene>
#include "C_GiNode.h"
#include "C_GiLiBusConnector.h"
#include "gitypes.h"
#include "C_OSCNode.h"
#include "C_OSCSystemBus.h"
#include "C_GtGetText.h"
#include "C_SebUtil.h"
#include "C_PuiSdUtil.h"
#include "C_Uti.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;
using namespace stw_types;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Set up GUI with all elements.

   \param[in]      oru64_ID         Unique ID
   \param[in]      orc_TriggerPos   Ideal position of connector
   \param[in]      opc_NodeItem     Start of connection at node
   \param[in]      opc_BusItem      End of connection at bus
   \param[in]      oq_MiddleLine    Indicator if middle line is required
   \param[in,out]  opc_Parent       Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiLiBusConnector::C_GiLiBusConnector(const uint64 & oru64_ID, const QPointF & orc_TriggerPos,
                                       C_GiNode * const opc_NodeItem, const C_GiLiBus * const opc_BusItem,
                                       const bool oq_MiddleLine, QGraphicsItem * const opc_Parent) :
   C_GiLiBusConnectorBase(oru64_ID, orc_TriggerPos, opc_BusItem, oq_MiddleLine, opc_Parent),
   mq_OnInteractionPointMoveFoundNode(false)
{
   // Update node and port
   this->m_InitConnector(opc_NodeItem, orc_TriggerPos);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Set up GUI with all elements.

   \param[in]      oru64_ID               Unique ID
   \param[in]      orc_InteractionPoints  Ideal interaction point positions
   \param[in]      opc_NodeItem           Start of connection at node
   \param[in]      opc_BusItem            End of connection at bus
   \param[in]      oq_MiddleLine          Indicator if middle line is required
   \param[in,out]  opc_Parent             Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiLiBusConnector::C_GiLiBusConnector(const uint64 & oru64_ID, const std::vector<QPointF> & orc_InteractionPoints,
                                       C_GiNode * const opc_NodeItem, const C_GiLiBus * const opc_BusItem,
                                       const bool oq_MiddleLine, QGraphicsItem * const opc_Parent) :
   C_GiLiBusConnectorBase(oru64_ID, orc_InteractionPoints, opc_BusItem, oq_MiddleLine, opc_Parent),
   mq_OnInteractionPointMoveFoundNode(false)
{
   std::vector<QPointF> c_SaveVec = orc_InteractionPoints;
   // Minimum two points are necessary. If vector has not enough points, add two points.
   if (c_SaveVec.size() < 2)
   {
      c_SaveVec.push_back(QPointF(0.0, 0.0));
      c_SaveVec.push_back(QPointF(0.0, 0.0));
   }

   // Update node and port
   this->m_InitConnector(opc_NodeItem, c_SaveVec[0]);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiLiBusConnector::~C_GiLiBusConnector(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the type of this item

   \return  ID
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_GiLiBusConnector::type(void) const
{
   return msn_GRAPHICS_ITEM_BUS_CONNECT;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Context menu entry delete connection was clicked
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBusConnector::DeleteConnection(void)
{
   C_GiPort * const pc_Port = dynamic_cast<C_GiPort *>(this->mpc_GenericPositionItem);

   if (pc_Port != NULL)
   {
      pc_Port->RemoveConnectorFromRegistry();
      this->mpc_GenericPositionItem = NULL;
   }
   this->mpc_GenericSignalItem = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set new point positions

   \param[in]  orc_ScenePos   New point position set
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBusConnector::SetPoints(const std::vector<QPointF> & orc_ScenePos)
{
   C_GiNode * const pc_Node = dynamic_cast<C_GiNode *>(this->mpc_GenericSignalItem);
   C_GiNode * const pc_LastNode = dynamic_cast<C_GiNode *>(this->mpc_LastKnownGenericSignalItem);
   const sint32 s32_Index = C_GiLiBusConnector::mh_GetGenericInteractionIndex();

   if (static_cast<uint32>(s32_Index) < orc_ScenePos.size())
   {
      this->m_UpdatePort(orc_ScenePos[s32_Index]);
   }
   C_GiLiBusConnectorBase::SetPoints(orc_ScenePos);
   this->m_UpdateExternal();
   //Port visibility
   if (pc_Node != NULL)
   {
      pc_Node->ReevaluatePortState();
   }
   if ((pc_LastNode != NULL) && (pc_LastNode != pc_Node))
   {
      pc_LastNode->ReevaluatePortState();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the node item

   \return  Node item
*/
//----------------------------------------------------------------------------------------------------------------------
const C_GiNode * C_GiLiBusConnector::GetNodeItem(void) const
{
   return dynamic_cast<const C_GiNode *>(mpc_GenericSignalItem);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the node item

   \return  Node item
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiNode * C_GiLiBusConnector::GetNodeItem(void)
{
   return dynamic_cast<C_GiNode *>(mpc_GenericSignalItem);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Write current GUI layer connection information to connector class.

   \param[in,out]  opc_UIConnection    Storage for GUI information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBusConnector::UpdateData(C_PuiSdNodeConnection * const opc_UIConnection) const
{
   if (opc_UIConnection != NULL)
   {
      const C_GiNode * const pc_Node = dynamic_cast<const C_GiNode *>(this->mpc_GenericSignalItem);
      //ID
      if (pc_Node != NULL)
      {
         const C_PuiSdNodeConnectionId * const pc_ID = pc_Node->GetNodeConnectionId(this);
         if (pc_ID != NULL)
         {
            opc_UIConnection->c_ConnectionID = *pc_ID;
         }
      }
      opc_UIConnection->c_UINodeConnectionInteractionPoints.clear();
      opc_UIConnection->c_UINodeConnectionInteractionPoints.reserve(this->mc_Points.size());
      for (QVector<C_GiLiInteractionPoint *>::const_iterator pc_ItPoint = this->mc_Points.begin();
           pc_ItPoint != this->mc_Points.end(); ++pc_ItPoint)
      {
         opc_UIConnection->c_UINodeConnectionInteractionPoints.push_back((*pc_ItPoint)->scenePos());
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Generate hint to display

   Connection to [Bus] using [Node] Interface [Interface number]
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBusConnector::GenerateHint(void)
{
   const C_GiNode * const pc_Node = dynamic_cast<const C_GiNode *>(this->mpc_GenericSignalItem);

   if ((pc_Node != NULL) && (this->GetBusItem() != NULL))
   {
      const C_PuiSdNodeConnectionId * const pc_NodeConnection = this->GetConnectionData();
      const C_OSCNode * pc_NodeData;
      pc_Node->GetOSCNodeConst(pc_NodeData);
      if ((pc_NodeConnection != NULL) && (pc_NodeData != NULL))
      {
         const C_OSCNodeComInterfaceSettings * const pc_ComInterfaceData = pc_NodeData->c_Properties.GetComInterface(
            pc_NodeConnection->e_InterfaceType, pc_NodeConnection->u8_InterfaceNumber);
         if (pc_ComInterfaceData != NULL)
         {
            QString c_Hint;
            // In case of Ethernet an empty string comes back
            QString c_TypeSpecificInfo = this->GetBusItem()->GetBitrate(true);

            if (c_TypeSpecificInfo == "")
            {
               // Get the IP address
               c_TypeSpecificInfo = ", IP: ";
               c_TypeSpecificInfo += C_Uti::h_IpAddressToString(pc_ComInterfaceData->c_Ip.au8_IpAddress);
            }

            //heading
            c_Hint = C_GtGetText::h_GetText("Bus Connection");
            this->SetDefaultToolTipHeading(c_Hint);

            //content
            c_Hint = static_cast<QString>(C_GtGetText::h_GetText(
                                             "%1 connected to %2 (Interface: %3%4)")).arg(
               pc_Node->GetText(),            //Node name
               this->GetBusItem()->GetName(), //Bus name
               C_PuiSdUtil::h_GetInterfaceName(this->GetBusItem()->GetType(),
                                               pc_NodeConnection->u8_InterfaceNumber), //Interface
               c_TypeSpecificInfo);                                                    // CAN bitrate or IP address
            this->SetDefaultToolTipContent(c_Hint);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get connection item

   \return
   Pointer to connection
   NULL: error = No data element found for connector
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiSdNodeConnectionId * C_GiLiBusConnector::GetConnectionData(void) const
{
   const C_PuiSdNodeConnectionId * pc_Retval = NULL;
   const C_GiNode * const pc_Node = dynamic_cast<const C_GiNode *>(this->mpc_GenericSignalItem);

   if (pc_Node != NULL)
   {
      pc_Retval = pc_Node->GetNodeConnectionId(this);
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Helper function to change bus connector interface number

   \param[in]  oru8_NewInterface    New interface number
   \param[in]  orc_Properties       Properties
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBusConnector::ChangeInterface(const uint8 & oru8_NewInterface,
                                         const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties)
{
   C_GiNode * const pc_Node = dynamic_cast<C_GiNode *>(this->mpc_GenericSignalItem);

   if (pc_Node != NULL)
   {
      pc_Node->ChangeInterface(oru8_NewInterface, this, orc_Properties);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Revert bus connection to last known node item

   \param[in,out]  opc_StartingNode    Last known node this bus connection was connected to
   \param[in,out]  opc_LastNode        New node this bus connection is currently connected to
   \param[in]      orc_ScenePos        Last known scene position of interaction point
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBusConnector::Revert(stw_opensyde_gui::C_GiNode * const opc_StartingNode,
                                const stw_opensyde_gui::C_GiNode * const opc_LastNode, const QPointF & orc_ScenePos)
{
   Q_UNUSED(opc_LastNode)
   this->mpc_GenericSignalItem = opc_StartingNode;
   m_UpdatePort(orc_ScenePos);
   m_Reconnect();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Connect bus connection to new node

   \param[in,out]  opc_StartingNode    Last known node this bus connection was connected to
   \param[in,out]  opc_LastNode        New node to connect this bus connection to
   \param[in]      orc_ConnectionPos   New connection position
   \param[in]      ors32_Interface     Interface number to use
   \param[in]      orc_Properties      Properties
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBusConnector::Reconnect(stw_opensyde_gui::C_GiNode * const opc_StartingNode,
                                   stw_opensyde_gui::C_GiNode * const opc_LastNode, const QPointF & orc_ConnectionPos,
                                   const stw_types::sint32 & ors32_Interface,
                                   const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties)
{
   this->mpc_GenericSignalItem = opc_LastNode;
   if (opc_LastNode != NULL)
   {
      if (this->GetBusItem() != NULL)
      {
         //Update data
         C_PuiSdNodeConnectionId c_NodeConn;
         c_NodeConn.e_InterfaceType = this->GetBusItem()->GetType();
         c_NodeConn.u8_InterfaceNumber = static_cast<uint8>(ors32_Interface);
         //Node

         if (opc_StartingNode != NULL)
         {
            //Delete connection for last node
            opc_StartingNode->RemoveConnector(this);
         }
         //Add connection to new node
         opc_LastNode->AddConnectionAndData(this, c_NodeConn, orc_Properties,
                                            static_cast<uint32>(this->GetBusItem()->GetIndex()));
         //Update tool tip
         Q_EMIT this->SigHintUpdate();
      }
      this->m_UpdatePort(orc_ConnectionPos);
      m_Reconnect();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Connect bus connection to new bus

   \param[in,out]  opc_StartingBus     Last known bus this bus connection was connected to
   \param[in,out]  opc_LastBus         New bus to connect this bus connection to
   \param[in]      orc_ConnectionPos   New connection position
   \param[in]      ors32_Interface     Interface number to use
   \param[in]      orc_Properties      Properties
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBusConnector::Reconnect(const stw_opensyde_gui::C_GiLiBus * const opc_StartingBus,
                                   const stw_opensyde_gui::C_GiLiBus * const opc_LastBus,
                                   const QPointF & orc_ConnectionPos, const stw_types::sint32 & ors32_Interface,
                                   const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties)
{
   Q_UNUSED(opc_StartingBus)
   this->m_SetBus(opc_LastBus);
   if (this->GetBusItem() != NULL)
   {
      C_GiNode * const pc_Node = dynamic_cast<C_GiNode *>(this->mpc_GenericSignalItem);
      //Update data
      C_PuiSdNodeConnectionId c_NodeConn;
      c_NodeConn.e_InterfaceType = this->GetBusItem()->GetType();
      c_NodeConn.u8_InterfaceNumber = static_cast<uint8>(ors32_Interface);
      //Bus
      if (pc_Node != NULL)
      {
         pc_Node->UpdateConnection(this, c_NodeConn, orc_Properties,
                                   static_cast<uint32>(this->GetBusItem()->GetIndex()));
      }
      //Update tool tip
      Q_EMIT this->SigHintUpdate();
   }
   this->m_CalcConnProgress(this->m_GetCurrentConn(), orc_ConnectionPos);
   m_Reconnect();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get current connected port

   \return
   Current connected port item
*/
//----------------------------------------------------------------------------------------------------------------------
const C_GiPort * C_GiLiBusConnector::GetPortItem(void) const
{
   return dynamic_cast<const C_GiPort *>(mpc_GenericPositionItem);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle interaction point move
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBusConnector::m_OnInteractionPointMove(void)
{
   this->mq_OnInteractionPointMoveFoundNode = false;
   C_GiLiBusConnectorBase::m_OnInteractionPointMove();
   if (mq_InAir == true)
   {
      //Check if active line is at border
      if ((this->msn_ActiveItemIndex == mh_GetGenericInteractionIndex()) ||
          (this->msn_ActiveItemIndex == m_GetBusInteractionIndex()))
      {
         if (this->msn_ActiveItemIndex == mh_GetGenericInteractionIndex())
         {
            if (this->mq_OnInteractionPointMoveFoundNode == false)
            {
               //Restore
               //Node
               m_UpdateGenericItem(dynamic_cast<C_GiNode *>(this->mpc_LastKnownGenericSignalItem));
               //Port
               if (this->mpc_GenericPositionItem != NULL)
               {
                  //If changed port unregister of starting point
                  if (this->mpc_GenericPositionItem != this->mpc_LastKnownGenericPositionItem)
                  {
                     C_GiPort * const pc_Port = dynamic_cast<C_GiPort *>(this->mpc_GenericPositionItem);
                     if (pc_Port != NULL)
                     {
                        pc_Port->RemoveConnectorFromRegistry();
                     }
                  }
               }
               this->mpc_GenericPositionItem = this->mpc_LastKnownGenericPositionItem;
               if (this->mpc_GenericPositionItem != NULL)
               {
                  C_GiPort * const pc_Port = dynamic_cast<C_GiPort *>(this->mpc_GenericPositionItem);
                  if (pc_Port != NULL)
                  {
                     //Restore visibility
                     pc_Port->AbortTemporaryUnregister();
                  }
               }
            }
            else
            {
               //Handle last known port if new port found
               if (this->mpc_LastKnownGenericPositionItem != NULL)
               {
                  if (this->mpc_GenericPositionItem != this->mpc_LastKnownGenericPositionItem)
                  {
                     C_GiPort * const pc_Port = dynamic_cast<C_GiPort *>(this->mpc_LastKnownGenericPositionItem);
                     if (pc_Port != NULL)
                     {
                        //Different port
                        pc_Port->TemporaryUnregister();
                     }
                  }
                  else
                  {
                     C_GiPort * const pc_Port = dynamic_cast<C_GiPort *>(this->mpc_LastKnownGenericPositionItem);
                     if (pc_Port != NULL)
                     {
                        //Same port
                        pc_Port->AbortTemporaryUnregister();
                     }
                  }
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle generic item interaction point move item iteration

   Active:
   * Iteration over current scene items
   * Generic interation point moving

   \param[in,out]  opc_HighestParentItem     Highest parent of current item
   \param[in]      orc_CurPos                Current hover position
   \param[in,out]  orq_RestoreMouseCursor    Restore mouse cursor flag (Only write false allowed if necessary)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBusConnector::m_OnIterationGenericInteractionPointMove(QGraphicsItem * const opc_HighestParentItem,
                                                                  const QPointF & orc_CurPos,
                                                                  bool & orq_RestoreMouseCursor)
{
   C_GiNode * const pc_Node = dynamic_cast<C_GiNode *>(opc_HighestParentItem);

   if (pc_Node != NULL)
   {
      if (pc_Node->isUnderMouse() == true)
      {
         if (this->GetBusItem() != NULL)
         {
            this->mc_Points[this->msn_ActiveItemIndex]->setCursor(pc_Node->cursor());
            orq_RestoreMouseCursor = false;
            if (pc_Node->CheckConnectionAvailable(this->GetBusItem()->GetType()) == true)
            {
               m_UpdateGenericItem(pc_Node);
               m_UpdatePort(orc_CurPos);
               this->mq_OnInteractionPointMoveFoundNode = true;
            }
            else
            {
               //Exception if same node (should always pass)
               if (pc_Node != this->mpc_LastKnownGenericSignalItem)
               {
                  Q_EMIT this->SigShowToolTip(orc_CurPos, pc_Node);
               }
               else
               {
                  m_UpdateGenericItem(pc_Node);
                  m_UpdatePort(orc_CurPos);
                  this->mq_OnInteractionPointMoveFoundNode = true;
               }
            }
         }
      }
   }
   else
   {
      if (opc_HighestParentItem != this)
      {
         C_GiBiCustomMouseItem * const pc_Other = dynamic_cast<C_GiBiCustomMouseItem *>(opc_HighestParentItem);
         if (((pc_Other != NULL) && (orq_RestoreMouseCursor == true)) &&
             (opc_HighestParentItem->isUnderMouse() == true))
         {
            this->mc_Points[this->msn_ActiveItemIndex]->setCursor(opc_HighestParentItem->cursor());
            orq_RestoreMouseCursor = false;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle generic interaction point mouse release

   \param[in]  orc_ScenePos   Scene position

   \return
   True  Default
   False No revert
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiLiBusConnector::m_OnGenericInteractionPointMouseRelease(const QPointF & orc_ScenePos)
{
   bool q_Retval = true;
   C_GiNode * const pc_Node = dynamic_cast<C_GiNode *>(this->mpc_GenericSignalItem);

   if (pc_Node != NULL)
   {
      if (this->mpc_GenericSignalItem != this->mpc_LastKnownGenericSignalItem)
      {
         if (pc_Node->contains(pc_Node->mapFromScene(orc_ScenePos)) == true)
         {
            Q_EMIT this->SigShowReconnectContextMenu(orc_ScenePos, pc_Node, this->GetBusItem(), -1, this);
            q_Retval = false;
         }
      }
      else
      {
         pc_Node->ReevaluatePortState();
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle bus change

   \param[in]  orc_ScenePos   Scene position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBusConnector::m_OnBusChange(const QPointF & orc_ScenePos)
{
   sint32 s32_Interface = -1;
   const C_PuiSdNodeConnectionId * const pc_Id = this->GetConnectionData();

   if (pc_Id != NULL)
   {
      s32_Interface = static_cast<sint32>(pc_Id->u8_InterfaceNumber);
   }
   Q_EMIT this->SigShowReconnectContextMenu(orc_ScenePos, dynamic_cast<C_GiNode *>(this->mpc_GenericSignalItem),
                                            this->GetBusItem(), s32_Interface, this);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init connector

   \param[in,out]  opc_NodeItem  Node to connect to
   \param[in]      orc_Pos       Position on bus
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBusConnector::m_InitConnector(C_GiNode * const opc_NodeItem, const QPointF & orc_Pos)
{
   //Node
   m_UpdateGenericItem(opc_NodeItem);

   //Port
   m_UpdatePort(orc_Pos);

   this->m_UpdateExternal();
   this->m_UpdateInternal();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle new port

   Find and save new port at position.
   Dismiss temporary port.

   \param[in]  orc_Pos  Position to evaluate for port
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBusConnector::m_UpdatePort(const QPointF & orc_Pos)
{
   C_GiPort * const pc_LastPort = dynamic_cast<C_GiPort *>(this->mpc_GenericPositionItem);
   C_GiNode * const pc_Node = dynamic_cast<C_GiNode *>(this->mpc_GenericSignalItem);

   if (pc_Node != NULL)
   {
      C_GiPort * pc_NewPort;
      pc_Node->FindClosestPort(orc_Pos, pc_NewPort);
      this->mpc_GenericPositionItem = pc_NewPort;
   }
   if (pc_LastPort != this->mpc_GenericPositionItem)
   {
      if ((pc_LastPort != NULL) && (pc_LastPort != this->mpc_LastKnownGenericPositionItem))
      {
         //Exception as last mpc_LastKnownPortItem should stay visible
         pc_LastPort->RemoveConnectorFromRegistry();
      }
      if ((this->mpc_GenericPositionItem != NULL) &&
          (this->mpc_GenericPositionItem != this->mpc_LastKnownGenericPositionItem))
      {
         C_GiPort * const pc_NewPort = dynamic_cast<C_GiPort *>(this->mpc_GenericPositionItem);
         if (pc_NewPort != NULL)
         {
            //Exception as last mpc_LastKnownPortItem should stay visible
            pc_NewPort->AddConnectorToRegistry();
         }
      }
   }
   if (this->mpc_GenericPositionItem != NULL)
   {
      m_CalcInitialLocalPos(orc_Pos);
   }
}
