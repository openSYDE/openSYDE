//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Graphics scene for system view update (implementation)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <cmath>
#include "C_Uti.h"
#include "C_GiSvPc.h"
#include "TGLUtils.h"
#include "stwerrors.h"
#include "C_OSCUtils.h"
#include "C_SyvUpScene.h"
#include "C_PuiSdHandler.h"
#include "C_PuiSvHandler.h"
#include "C_OSCLoggingHandler.h"
#include "C_GiSvNodeSyvUpdate.h"
#include "C_GiBiConnectableItem.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const stw_types::float64 C_SyvUpScene::mhf64_BusAnimationTolerance = 0.001;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in]     ou32_ViewIndex View index
   \param[in,out] opc_Parent     Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpScene::C_SyvUpScene(const uint32 ou32_ViewIndex, QObject * const opc_Parent) :
   C_SyvTopologyBaseScene(ou32_ViewIndex, opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpScene::~C_SyvUpScene()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update connected status

   \param[in] oq_Active      Flag if connected
   \param[in] oq_SignalNodes Optional flag to signal nodes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpScene::SetConnected(const bool oq_Active, const bool oq_SignalNodes) const
{
   const QList<QGraphicsItem *> c_Items = this->items();

   //Search for matching bus
   for (QList<QGraphicsItem *>::const_iterator c_ItItem = c_Items.begin(); c_ItItem != c_Items.end(); ++c_ItItem)
   {
      QGraphicsItem * const pc_CurItemParent = C_SebUtil::h_GetHighestParent(*c_ItItem);
      if (pc_CurItemParent != NULL)
      {
         //Nodes

         C_GiSvNodeSyvUpdate * const pc_Node = dynamic_cast<C_GiSvNodeSyvUpdate *>(pc_CurItemParent);
         if ((pc_Node != NULL) && (oq_SignalNodes == true))
         {
            pc_Node->SetConnected(oq_Active);
         }
         else
         {
            C_GiSvPc * const pc_Pc = dynamic_cast<C_GiSvPc *>(pc_CurItemParent);
            if (pc_Pc != NULL)
            {
               pc_Pc->SetConnected(oq_Active);
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Signal if update in progress

   \param[in] oq_Active Flag if update in progress
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpScene::SetUpdating(const bool oq_Active) const
{
   const QList<QGraphicsItem *> c_Items = this->items();

   //Search for matching bus
   for (QList<QGraphicsItem *>::const_iterator c_ItItem = c_Items.begin(); c_ItItem != c_Items.end(); ++c_ItItem)
   {
      QGraphicsItem * const pc_CurItemParent = C_SebUtil::h_GetHighestParent(*c_ItItem);
      if (pc_CurItemParent != NULL)
      {
         //Nodes

         C_GiSvNodeSyvUpdate * const pc_Node = dynamic_cast<C_GiSvNodeSyvUpdate *>(pc_CurItemParent);
         if (pc_Node != NULL)
         {
            pc_Node->SetUpdating(oq_Active);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update device information

   \param[in] orc_NodeIndexes       Node indices
   \param[in] orc_DeviceInformation Device info
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpScene::UpdateDeviceInformation(const std::vector<uint32> & orc_NodeIndexes,
                                           const std::vector<C_SyvUpDeviceInfo> & orc_DeviceInformation) const
{
   tgl_assert(orc_NodeIndexes.size() == orc_DeviceInformation.size());
   if (orc_NodeIndexes.size() == orc_DeviceInformation.size())
   {
      const QList<QGraphicsItem *> c_Items = this->items();

      //Search for matching bus
      for (QList<QGraphicsItem *>::const_iterator c_ItItem = c_Items.begin(); c_ItItem != c_Items.end(); ++c_ItItem)
      {
         QGraphicsItem * const pc_CurItemParent = C_SebUtil::h_GetHighestParent(*c_ItItem);
         if (pc_CurItemParent != NULL)
         {
            //Nodes

            C_GiSvNodeSyvUpdate * const pc_Node = dynamic_cast<C_GiSvNodeSyvUpdate *>(pc_CurItemParent);
            if (pc_Node != NULL)
            {
               for (uint32 u32_ItDevice = 0; u32_ItDevice < orc_NodeIndexes.size(); ++u32_ItDevice)
               {
                  if (static_cast<uint32>(pc_Node->GetIndex()) == orc_NodeIndexes[u32_ItDevice])
                  {
                     pc_Node->UpdateInitialPackageStatus(orc_DeviceInformation[u32_ItDevice]);

                     break;
                  }
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Start connection animation
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpScene::StartConnectionAnimation(void) const
{
   const QList<QGraphicsItem *> c_Items = this->items();

   //Search for matching bus
   for (QList<QGraphicsItem *>::const_iterator c_ItItem = c_Items.begin(); c_ItItem != c_Items.end(); ++c_ItItem)
   {
      QGraphicsItem * const pc_CurItemParent = C_SebUtil::h_GetHighestParent(*c_ItItem);
      if (pc_CurItemParent != NULL)
      {
         //PC connection

         C_GiSvPcBusConnector * const pc_CurPcConnector = dynamic_cast<C_GiSvPcBusConnector *>(pc_CurItemParent);
         if (pc_CurPcConnector != NULL)
         {
            bool q_SpeedUp = false;
            if ((pc_CurPcConnector->GetBusItem() != NULL) &&
                (pc_CurPcConnector->GetBusItem()->GetType() == C_OSCSystemBus::eETHERNET))
            {
               q_SpeedUp = true;
            }
            //Always animated
            pc_CurPcConnector->SetAnimated(true, true, q_SpeedUp);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Start progress animation

   \param[out] ou32_NodeIndex SD node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpScene::StartProgressAnimation(const uint32 ou32_NodeIndex) const
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
   const C_SyvRoRouteCalculation c_Routing(this->mu32_ViewIndex, ou32_NodeIndex, C_OSCRoutingCalculation::eUPDATE);
   const C_OSCRoutingRoute * const pc_Route = c_Routing.GetBestRoute();

   if ((pc_Route != NULL) && (pc_View != NULL))
   {
      const QList<QGraphicsItem *> c_Items = this->items();

      //Search for matching bus
      for (QList<QGraphicsItem *>::const_iterator c_ItItem = c_Items.begin(); c_ItItem != c_Items.end(); ++c_ItItem)
      {
         QGraphicsItem * const pc_CurItemParent = C_SebUtil::h_GetHighestParent(*c_ItItem);
         if (pc_CurItemParent != NULL)
         {
            //Connectors

            C_GiLiBusConnector * const pc_CurBusConnector = dynamic_cast<C_GiLiBusConnector *>(pc_CurItemParent);
            if (pc_CurBusConnector != NULL)
            {
               const C_GiLiBus * const pc_ConnectedBus = pc_CurBusConnector->GetBusItem();
               const C_GiNode * const pc_ConnectedNode = pc_CurBusConnector->GetNodeItem();
               const C_PuiSdNodeConnectionId * const pc_ConnectionData = pc_CurBusConnector->GetConnectionData();
               if (((pc_ConnectedBus != NULL) && (pc_ConnectedNode != NULL)) && (pc_ConnectionData != NULL))
               {
                  //On route
                  bool q_Inverse = false;
                  bool q_FoundOnRoute = false;
                  //Check if target node
                  if (static_cast<uint32>(pc_ConnectedNode->GetIndex()) == ou32_NodeIndex)
                  {
                     const C_OSCNode * const pc_NodeData = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(
                        ou32_NodeIndex);
                     if (pc_NodeData != NULL)
                     {
                        uint32 u32_BusIndexToSearchFor = 0;
                        if (pc_Route->c_VecRoutePoints.size() > 0)
                        {
                           //Search for last bus before the target node
                           const C_OSCRoutingRoutePoint & rc_LastHop =
                              pc_Route->c_VecRoutePoints[static_cast<std::vector< C_OSCRoutingRoutePoint>::size_type >
                                                         (pc_Route->c_VecRoutePoints.size() - 1UL)];
                           u32_BusIndexToSearchFor = rc_LastHop.u32_OutBusIndex;
                        }
                        else
                        {
                           //Search for PC bus if no routing necessary
                           u32_BusIndexToSearchFor = pc_View->GetPcData().GetBusIndex();
                        }
                        //Search for first viable interface
                        if ((pc_ConnectedBus->GetIndex() >= 0) &&
                            (static_cast<uint32>(pc_ConnectedBus->GetIndex()) == u32_BusIndexToSearchFor))
                        {
                           for (uint32 u32_ItInterface = 0;
                                u32_ItInterface < pc_NodeData->c_Properties.c_ComInterfaces.size(); ++u32_ItInterface)
                           {
                              const C_OSCNodeComInterfaceSettings & rc_ComInterface =
                                 pc_NodeData->c_Properties.c_ComInterfaces[u32_ItInterface];
                              if ((rc_ComInterface.q_IsBusConnected == true) &&
                                  (rc_ComInterface.u32_BusIndex == u32_BusIndexToSearchFor))
                              {
                                 //Check if routing engine would choose this interface
                                 if (c_Routing.CheckItfNumberForRouting(ou32_NodeIndex, rc_ComInterface) == true)
                                 {
                                    if (rc_ComInterface.u8_InterfaceNumber == pc_ConnectionData->u8_InterfaceNumber)
                                    {
                                       q_FoundOnRoute = true;
                                       //Stop after first one
                                       break;
                                    }
                                 }
                              }
                           }
                        }
                     }
                  }
                  else
                  {
                     for (uint32 u32_ItRoute = 0; u32_ItRoute < pc_Route->c_VecRoutePoints.size(); ++u32_ItRoute)
                     {
                        const C_OSCRoutingRoutePoint & rc_Point = pc_Route->c_VecRoutePoints[u32_ItRoute];
                        //Check in or out
                        if ((static_cast<uint32>(pc_ConnectedNode->GetIndex()) == rc_Point.u32_NodeIndex) &&
                            (((static_cast<uint32>(pc_ConnectedBus->GetIndex()) == rc_Point.u32_InBusIndex) && (
                                 pc_ConnectionData->u8_InterfaceNumber == rc_Point.u8_InInterfaceNumber)) ||
                             ((static_cast<uint32>(pc_ConnectedBus->GetIndex()) == rc_Point.u32_OutBusIndex) && (
                                 pc_ConnectionData->u8_InterfaceNumber == rc_Point.u8_OutInterfaceNumber))))
                        {
                           if (static_cast<uint32>(pc_ConnectedBus->GetIndex()) == rc_Point.u32_InBusIndex)
                           {
                              q_Inverse = false;
                           }
                           else
                           {
                              q_Inverse = true;
                           }
                           q_FoundOnRoute = true;
                           break;
                        }
                     }
                  }
                  if (q_FoundOnRoute == false)
                  {
                     //Deactivate
                     pc_CurBusConnector->SetAnimated(false, q_Inverse);
                  }
                  else
                  {
                     bool q_SpeedUp = false;
                     if ((pc_CurBusConnector->GetBusItem() != NULL) &&
                         (pc_CurBusConnector->GetBusItem()->GetType() == C_OSCSystemBus::eETHERNET))
                     {
                        q_SpeedUp = true;
                     }
                     //Activate
                     pc_CurBusConnector->SetAnimated(true, q_Inverse, q_SpeedUp);
                  }
               }
            }
            else
            {
               //PC connection

               C_GiSvPcBusConnector * const pc_CurPcConnector =
                  dynamic_cast<C_GiSvPcBusConnector *>(pc_CurItemParent);
               if (pc_CurPcConnector != NULL)
               {
                  bool q_SpeedUp = false;
                  if ((pc_CurPcConnector->GetBusItem() != NULL) &&
                      (pc_CurPcConnector->GetBusItem()->GetType() == C_OSCSystemBus::eETHERNET))
                  {
                     q_SpeedUp = true;
                  }
                  //Always animated
                  pc_CurPcConnector->SetAnimated(true, true, q_SpeedUp);
               }
               else
               {
                  //Nodes

                  C_GiSvNodeSyvUpdate * const pc_Node =
                     dynamic_cast<C_GiSvNodeSyvUpdate *>(pc_CurItemParent);
                  if (pc_Node != NULL)
                  {
                     //Check if currently updated node
                     if (ou32_NodeIndex == static_cast<uint32>(pc_Node->GetIndex()))
                     {
                        pc_Node->SetNodeUpdateInProgress(true, false, 0);
                     }
                     else
                     {
                        pc_Node->SetNodeUpdateInProgress(false, false, 0);
                     }
                  }
               }
            }
         }
      }
   }
   if (m_StartProgressAnimationBusses(c_Routing, ou32_NodeIndex) != C_NO_ERR)
   {
      osc_write_log_info("Update animation", "Animation failure");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Stop all progress animations

   \param[in] oq_Abort                    Flag if action aborted
   \param[in] ou32_FailedApplicationIndex If aborted and currently updating this is the currently updated application
   \param[in] oq_StopUpdateingState       Flag to stop updating state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpScene::StopProgressAnimation(const bool oq_Abort, const uint32 ou32_FailedApplicationIndex,
                                         const bool oq_StopUpdateingState) const
{
   const QList<QGraphicsItem *> c_Items = this->items();

   if (oq_StopUpdateingState == true)
   {
      this->SetUpdating(false);
   }
   //Search for matching bus
   for (QList<QGraphicsItem *>::const_iterator c_ItItem = c_Items.begin(); c_ItItem != c_Items.end(); ++c_ItItem)
   {
      QGraphicsItem * const pc_CurItemParent = C_SebUtil::h_GetHighestParent(*c_ItItem);
      if (pc_CurItemParent != NULL)
      {
         //Busses

         C_GiLiBus * const pc_CurBus = dynamic_cast<C_GiLiBus *>(pc_CurItemParent);
         if (pc_CurBus != NULL)
         {
            //Deactivate
            pc_CurBus->SetAnimated(false);
         }
         else
         {
            //Connectors

            C_GiLiBusConnector * const pc_CurBusConnector = dynamic_cast<C_GiLiBusConnector *>(pc_CurItemParent);
            if (pc_CurBusConnector != NULL)
            {
               //Deactivate
               pc_CurBusConnector->SetAnimated(false);
            }
            else
            {
               //PC connection

               C_GiSvPcBusConnector * const pc_CurPcConnector =
                  dynamic_cast<C_GiSvPcBusConnector *>(pc_CurItemParent);
               if (pc_CurPcConnector != NULL)
               {
                  //Deactivate
                  pc_CurPcConnector->SetAnimated(false);
               }
               else
               {
                  //Nodes

                  C_GiSvNodeSyvUpdate * const pc_Node =
                     dynamic_cast<C_GiSvNodeSyvUpdate *>(pc_CurItemParent);
                  if (pc_Node != NULL)
                  {
                     pc_Node->SetNodeUpdateInProgress(false, oq_Abort, ou32_FailedApplicationIndex);
                  }
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set no response state for node

   \param[in] ou32_NodeIndex Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpScene::SetNoResponse(const uint32 ou32_NodeIndex) const
{
   const QList<QGraphicsItem *> c_Items = this->items();

   //Search for matching bus
   for (QList<QGraphicsItem *>::const_iterator c_ItItem = c_Items.begin(); c_ItItem != c_Items.end(); ++c_ItItem)
   {
      QGraphicsItem * const pc_CurItemParent = C_SebUtil::h_GetHighestParent(*c_ItItem);
      if (pc_CurItemParent != NULL)
      {
         //Nodes

         C_GiSvNodeSyvUpdate * const pc_Node = dynamic_cast<C_GiSvNodeSyvUpdate *>(pc_CurItemParent);
         if ((pc_Node != NULL) && (static_cast<uint32>(pc_Node->GetIndex()) == ou32_NodeIndex))
         {
            pc_Node->SetNoResponse();
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all not responding node indices

   \return
   All not responding node indices
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<uint32> C_SyvUpScene::GetActiveNoResponseNodeIndices(void) const
{
   const QList<QGraphicsItem *> c_Items = this->items();

   std::vector<uint32> c_Retval;

   for (QList<QGraphicsItem *>::const_iterator c_ItItem = c_Items.begin(); c_ItItem != c_Items.end(); ++c_ItItem)
   {
      const QGraphicsItem * const pc_CurItemParent = C_SebUtil::h_GetHighestParent(*c_ItItem);
      if (pc_CurItemParent != NULL)
      {
         //Nodes

         const C_GiSvNodeSyvUpdate * const pc_Node = dynamic_cast<const C_GiSvNodeSyvUpdate *>(pc_CurItemParent);
         if (((pc_Node != NULL) && (pc_Node->GetIndex() >= 0)) && (pc_Node->HasNoResponseAndIsActive() == true))
         {
            c_Retval.push_back(static_cast<uint32>(pc_Node->GetIndex()));
         }
      }
   }
   //Nodes may have been added multiple times
   C_Uti::h_Uniqueify(c_Retval);
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all none third party node indices

   \return
   All none third party node indices
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<uint32> C_SyvUpScene::GetActiveNoneThirdPartyNodeIndices(void) const
{
   const QList<QGraphicsItem *> c_Items = this->items();

   std::vector<uint32> c_Retval;

   for (QList<QGraphicsItem *>::const_iterator c_ItItem = c_Items.begin(); c_ItItem != c_Items.end(); ++c_ItItem)
   {
      const QGraphicsItem * const pc_CurItemParent = C_SebUtil::h_GetHighestParent(*c_ItItem);
      if (pc_CurItemParent != NULL)
      {
         //Nodes

         const C_GiSvNodeSyvUpdate * const pc_Node = dynamic_cast<const C_GiSvNodeSyvUpdate *>(pc_CurItemParent);
         if ((((pc_Node != NULL) && (pc_Node->GetIndex() >= 0)) && (pc_Node->IsActiveInView() == true)) &&
             (pc_Node->IsStwDevice() == true))
         {
            c_Retval.push_back(static_cast<uint32>(pc_Node->GetIndex()));
         }
      }
   }
   //Nodes may have been added multiple times
   C_Uti::h_Uniqueify(c_Retval);
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Trigger update icon check for all nodes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpScene::CheckUpdateDisabledState(void) const
{
   const QList<QGraphicsItem *> c_Items = this->items();

   for (QList<QGraphicsItem *>::const_iterator c_ItItem = c_Items.begin(); c_ItItem != c_Items.end(); ++c_ItItem)
   {
      QGraphicsItem * const pc_CurItemParent = C_SebUtil::h_GetHighestParent(*c_ItItem);
      if (pc_CurItemParent != NULL)
      {
         //Nodes

         C_GiSvNodeSyvUpdate * const pc_Node = dynamic_cast<C_GiSvNodeSyvUpdate *>(pc_CurItemParent);
         if (((pc_Node != NULL) && (pc_Node->GetIndex() >= 0)) && (pc_Node->IsActiveInView() == true))
         {
            pc_Node->UpdateIcons();
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checking if any item can be added to the scene

   \return
   true     Items can be added
   false    Items cannot be added
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpScene::IsAnyItemAddable(void) const
{
   return false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checking if the graphics item is movable on the scene

   \param[in]  opc_Item    Graphics item to check

   \return
   true     Item is movable
   false    Item is not movable
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpScene::IsItemMovable(const QGraphicsItem * const opc_Item) const
{
   Q_UNUSED(opc_Item)
   return false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checking if the graphics item is selectable on the scene

   \param[in]  opc_Item    Graphics item to check

   \return
   true     Item is selectable
   false    Item is not selectable
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpScene::IsItemSelectable(const QGraphicsItem * const opc_Item) const
{
   Q_UNUSED(opc_Item)
   return false;
}

//----------------------------------------------------------------------------------------------------------------------
C_SebBaseContextMenuManager * C_SyvUpScene::m_GetContextMenuManager(void)
{
   return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
C_SebUnoBaseManager * C_SyvUpScene::m_GetUndoManager(void)
{
   return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
C_GiNode * C_SyvUpScene::m_CreateNode(const sint32 & ors32_Index, const uint64 & oru64_ID, const float64 & orf64_Width,
                                      const float64 & orf64_Height, QGraphicsItem * const opc_Parent)
{
   return new C_GiSvNodeSyvUpdate(this->mu32_ViewIndex, ors32_Index, oru64_ID, orf64_Width, orf64_Height, opc_Parent);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse double click event slot

   Here: handle item edit

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * const opc_Event)
{
   QGraphicsItem * const pc_Item = this->itemAt(opc_Event->scenePos(), QTransform());

   if ((pc_Item != NULL) &&
       (opc_Event->buttons().testFlag(Qt::LeftButton) == true))
   {
      QGraphicsItem * const pc_Parent = C_SebUtil::h_GetHighestParent(pc_Item);

      if (pc_Parent != NULL)
      {
         //Nodes

         C_GiSvNodeSyvUpdate * const pc_Node = dynamic_cast<C_GiSvNodeSyvUpdate *>(pc_Parent);
         if (pc_Node != NULL)
         {
            pc_Node->ShowInfo();
         }
         else
         {
            C_GiSvPc * const pc_Pc = dynamic_cast<C_GiSvPc * const>(pc_Parent);

            if (pc_Pc != NULL)
            {
               pc_Pc->OpenDialog();
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new node to scene and connect signals

   \param[in,out] opc_NodeGraphicsItem Pointer to new node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpScene::m_AddNodeToScene(C_GiNode * const opc_NodeGraphicsItem)
{
   C_GiSvNodeSyvUpdate * const pc_UpdateNode = dynamic_cast<C_GiSvNodeSyvUpdate * const>(opc_NodeGraphicsItem);

   if (pc_UpdateNode != NULL)
   {
      connect(pc_UpdateNode, &C_GiSvNodeSyvUpdate::SigDiscardInfo, this, &C_SyvUpScene::SigDiscardInfo);
   }
   C_SyvTopologyBaseScene::m_AddNodeToScene(opc_NodeGraphicsItem);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle bus progress animation start

   \param[in] orc_Calc        Routes
   \param[in] ou32_NodeIndex  Node index

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvUpScene::m_StartProgressAnimationBusses(const C_SyvRoRouteCalculation & orc_Calc,
                                                    const uint32 ou32_NodeIndex) const
{
   sint32 s32_Retval = C_NO_ERR;
   const C_OSCNode * const pc_OSCNode = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(ou32_NodeIndex);
   const C_PuiSdNode * const pc_UiNode = C_PuiSdHandler::h_GetInstance()->GetUINode(ou32_NodeIndex);
   const C_OSCRoutingRoute * const pc_Route = orc_Calc.GetBestRoute();
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   if (((pc_UiNode != NULL) && (pc_OSCNode != NULL)) &&
       (((pc_Route != NULL) && (pc_View != NULL)) && (pc_View->GetPcData().GetConnected() == true)))
   {
      //Internal knowledge: Index of connector interaction point on bus is: LAST index
      //Special cases
      //0: PC to bus 0
      //buses.size()-1: Bus to node index
      //Default (if buses size > 1): bus to node, node to bus+1 -> Routing
      //Animated buses
      std::vector<uint32> c_Busses;
      //Segments of buses (always double the bus count)
      std::vector<QPointF> c_BusSegments;
      //Other
      const std::vector<QPointF> & rc_PCConnectionPoints =
         pc_View->GetPcData().GetConnectionData().c_UIInteractionPoints;
      //Reserve
      c_Busses.reserve(static_cast<std::vector<uint32>::size_type>(pc_Route->c_VecRoutePoints.size() + 1UL));
      c_BusSegments.reserve(static_cast<std::vector<QPointF>::size_type>((pc_Route->c_VecRoutePoints.size() + 1UL) *
                                                                         2UL));
      //Add PC bus
      c_Busses.push_back(pc_View->GetPcData().GetBusIndex());
      //Add route end points
      for (uint32 u32_ItRoute = 0; u32_ItRoute < pc_Route->c_VecRoutePoints.size(); ++u32_ItRoute)
      {
         const C_OSCRoutingRoutePoint & rc_Point = pc_Route->c_VecRoutePoints[u32_ItRoute];
         c_Busses.push_back(rc_Point.u32_OutBusIndex);
      }
      //Handle first point
      if (rc_PCConnectionPoints.size() > 0UL)
      {
         c_BusSegments.push_back(rc_PCConnectionPoints[static_cast<std::vector< QPointF>::size_type >
                                                       (rc_PCConnectionPoints.size() - 1UL)]);
         //Handle intermediate points
         for (uint32 u32_ItRoute = 0; u32_ItRoute < pc_Route->c_VecRoutePoints.size(); ++u32_ItRoute)
         {
            const C_OSCRoutingRoutePoint & rc_Point = pc_Route->c_VecRoutePoints[u32_ItRoute];
            const C_PuiSdNodeConnection * const pc_InConnection = C_PuiSdHandler::h_GetInstance()->GetUiConnection(
               rc_Point.u32_NodeIndex, rc_Point.u8_InInterfaceNumber, rc_Point.e_InInterfaceType);
            const C_PuiSdNodeConnection * const pc_OutConnection = C_PuiSdHandler::h_GetInstance()->GetUiConnection(
               rc_Point.u32_NodeIndex, rc_Point.u8_OutInterfaceNumber, rc_Point.e_OutInterfaceType);
            if ((pc_InConnection != NULL) && (pc_OutConnection != NULL))
            {
               const std::vector<QPointF> & rc_InConnectionPoints =
                  pc_InConnection->c_UINodeConnectionInteractionPoints;
               const std::vector<QPointF> & rc_OutConnectionPoints =
                  pc_OutConnection->c_UINodeConnectionInteractionPoints;
               //Add bis interaction points
               //In FIRST
               if (rc_InConnectionPoints.size() > 0UL)
               {
                  c_BusSegments.push_back(rc_InConnectionPoints[static_cast<std::vector< QPointF>::size_type >
                                                                (rc_InConnectionPoints.size() - 1UL)]);
               }
               //Out
               if (rc_OutConnectionPoints.size() > 0UL)
               {
                  c_BusSegments.push_back(rc_OutConnectionPoints[static_cast<std::vector< QPointF>::size_type >
                                                                 (rc_OutConnectionPoints.size() - 1UL)]);
               }
            }
         }
         //Handle last point
         //Two steps: first search for correct interface, then search for ui data of interface
         for (uint32 u32_ItInterface = 0; u32_ItInterface < pc_OSCNode->c_Properties.c_ComInterfaces.size();
              ++u32_ItInterface)
         {
            const C_OSCNodeComInterfaceSettings & rc_Interface =
               pc_OSCNode->c_Properties.c_ComInterfaces[u32_ItInterface];
            if ((rc_Interface.q_IsBusConnected == true) &&
                (c_Busses[static_cast<std::vector< uint32>::size_type > (c_Busses.size() - 1UL)] ==
                 rc_Interface.u32_BusIndex))
            {
               const C_PuiSdNodeConnection * const pc_UiConnectionData =
                  C_PuiSdHandler::h_GetInstance()->GetUiConnection(ou32_NodeIndex, u32_ItInterface,
                                                                   rc_Interface.e_InterfaceType);
               if (pc_UiConnectionData != NULL)
               {
                  const std::vector<QPointF> & rc_ConnectionPoints =
                     pc_UiConnectionData->c_UINodeConnectionInteractionPoints;
                  if (rc_ConnectionPoints.size() > 0UL)
                  {
                     c_BusSegments.push_back(rc_ConnectionPoints[static_cast<std::vector< QPointF>::size_type >
                                                                 (rc_ConnectionPoints.size() - 1UL)]);
                  }
                  break;
               }
            }
         }
         if (c_BusSegments.size() == (c_Busses.size() * 2UL))
         {
            for (uint32 u32_ItBus = 0; u32_ItBus < c_Busses.size(); ++u32_ItBus)
            {
               const C_PuiSdBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetUIBus(c_Busses[u32_ItBus]);
               if (pc_Bus != NULL)
               {
                  QPolygonF c_AnimatedPoints;
                  bool q_Inverse;
                  const QPointF & rc_StartPoint = c_BusSegments[static_cast<std::vector< QPointF>::size_type >
                                                                (u32_ItBus * 2UL)];
                  const QPointF & rc_EndPoint = c_BusSegments[static_cast<std::vector< QPointF>::size_type >
                                                              ((u32_ItBus * 2UL) + 1UL)];

                  if (mh_GetAnimationPath(rc_StartPoint, rc_EndPoint, pc_Bus->c_UIInteractionPoints, c_AnimatedPoints,
                                          q_Inverse) == C_NO_ERR)
                  {
                     //Apply path
                     const QList<QGraphicsItem *> c_Items = this->items();

                     //Search for matching bus
                     for (QList<QGraphicsItem *>::const_iterator c_ItItem = c_Items.begin(); c_ItItem != c_Items.end();
                          ++c_ItItem)
                     {
                        QGraphicsItem * const pc_CurItemParent = C_SebUtil::h_GetHighestParent(*c_ItItem);
                        if (pc_CurItemParent != NULL)
                        {
                           //Busses

                           C_GiLiBus * const pc_CurBus = dynamic_cast<C_GiLiBus *>(pc_CurItemParent);
                           if ((pc_CurBus != NULL) &&
                               (static_cast<uint32>(pc_CurBus->GetIndex()) == c_Busses[u32_ItBus]))
                           {
                              bool q_SpeedUp = false;
                              if (pc_CurBus->GetType() == C_OSCSystemBus::eETHERNET)
                              {
                                 q_SpeedUp = true;
                              }
                              //Deactivate
                              //Somehow inverse has to be inverted
                              pc_CurBus->SetAnimated(true, !q_Inverse, q_SpeedUp, c_AnimatedPoints);
                           }
                        }
                     }
                  }
                  else
                  {
                     s32_Retval = C_CONFIG;
                  }
               }
               else
               {
                  s32_Retval = C_CONFIG;
               }
            }
         }
         else
         {
            s32_Retval = C_CONFIG;
         }
      }
      else
      {
         s32_Retval = C_CONFIG;
      }
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculate animation path

   \param[in]  orc_PointStart          Animation start point
   \param[in]  orc_PointEnd            Animation end point
   \param[in]  orc_UIInteractionPoints Bus interaction points
   \param[out] orc_Path                Calculated animation path
   \param[out] orq_Inverse             Flag if animation inverse (Necessary for start after end)

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvUpScene::mh_GetAnimationPath(const QPointF & orc_PointStart, const QPointF & orc_PointEnd,
                                         const std::vector<QPointF> & orc_UIInteractionPoints, QPolygonF & orc_Path,
                                         bool & orq_Inverse)
{
   sint32 s32_Retval;
   bool q_First = true;

   orq_Inverse = false;
   bool q_Finished = false;

   //Path calculation
   for (uint32 u32_ItBusConnection = 0;
        (u32_ItBusConnection < (orc_UIInteractionPoints.size() - 1UL)) && (q_Finished == false);
        ++u32_ItBusConnection)
   {
      const QPointF & rc_LineStart = orc_UIInteractionPoints[u32_ItBusConnection];
      const QPointF & rc_LineEnd =
         orc_UIInteractionPoints[static_cast<std::vector< QPointF>::size_type > (u32_ItBusConnection + 1UL)];
      float64 f64_Dist1;
      float64 f64_Dist2;
      C_GiBiConnectableItem::h_DistToLine(rc_LineStart, rc_LineEnd, orc_PointStart, &f64_Dist1);
      C_GiBiConnectableItem::h_DistToLine(rc_LineStart, rc_LineEnd, orc_PointEnd, &f64_Dist2);
      if (std::abs(f64_Dist1) < C_SyvUpScene::mhf64_BusAnimationTolerance)
      {
         if (std::abs(f64_Dist2) < C_SyvUpScene::mhf64_BusAnimationTolerance)
         {
            //Undecided
            C_GiBiConnectableItem::h_DistToPoint(rc_LineStart, orc_PointStart, f64_Dist1);
            C_GiBiConnectableItem::h_DistToPoint(rc_LineStart, orc_PointEnd, f64_Dist2);
            q_Finished = true;
            if (f64_Dist1 < f64_Dist2)
            {
               orq_Inverse = false;
               orc_Path.push_back(orc_PointStart);
               orc_Path.push_back(orc_PointEnd);
            }
            else
            {
               orq_Inverse = true;
               orc_Path.push_back(orc_PointEnd);
               orc_Path.push_back(orc_PointStart);
            }
         }
         else
         {
            if (q_First == true)
            {
               //Conn1 first
               q_First = false;
               orq_Inverse = false;
               orc_Path.push_back(orc_PointStart);
               orc_Path.push_back(rc_LineEnd);
            }
            else
            {
               //Finished
               orc_Path.push_back(orc_PointStart);
               q_Finished = true;
            }
         }
      }
      else
      {
         if (std::abs(f64_Dist2) < C_SyvUpScene::mhf64_BusAnimationTolerance)
         {
            if (q_First == true)
            {
               //Conn2 first
               q_First = false;
               orq_Inverse = true;
               orc_Path.push_back(orc_PointEnd);
               orc_Path.push_back(rc_LineEnd);
            }
            else
            {
               //Finished
               orc_Path.push_back(orc_PointEnd);
               q_Finished = true;
            }
         }
         else
         {
            //After one point was added
            if (q_First == false)
            {
               //Add only line end point (start already added)
               orc_Path.push_back(rc_LineEnd);
            }
         }
      }
   }
   if (q_Finished == true)
   {
      s32_Retval = C_NO_ERR;
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}
