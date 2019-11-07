//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Common graphics scene with topology functionality (implementation)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QApplication>

#include "gitypes.h"

#include "C_SebTopologyBaseScene.h"

#include "C_GiLiLineGroup.h"
#include "C_GiArrowCursorButton.h"
#include "C_GiLiBus.h"
#include "C_GiLiEthernetBus.h"
#include "C_GiLiCANBus.h"

#include "C_PuiSdHandler.h"
#include "C_OSCNode.h"
#include "C_OSCSystemBus.h"

#include "C_SebUtil.h"
#include "C_SebUnoTopBaseManager.h"
#include "C_GiSdBoundary.h"
#include "C_GiSdArrow.h"
#include "C_GiSdTextElement.h"
#include "C_GiSdImageGroup.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SebTopologyBaseScene::C_SebTopologyBaseScene(QObject * const opc_Parent) :
   C_SebScene(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SebTopologyBaseScene::~C_SebTopologyBaseScene()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load system definition
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebTopologyBaseScene::Load(void)
{
   C_SebUnoBaseManager * const pc_UndoManager = this->m_GetUndoManager();

   QApplication::setOverrideCursor(Qt::WaitCursor);
   //Clear scene manually (some items may not be cleared
   m_Clear();

   this->m_LoadProject();

   if (pc_UndoManager != NULL)
   {
      pc_UndoManager->clear();
   }
   QApplication::restoreOverrideCursor();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Checking the nodes for errors
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebTopologyBaseScene::CheckAllItemsForChanges(void) const
{
   QList<QGraphicsItem *> c_Items = this->items();
   for (QList<QGraphicsItem *>::iterator c_ItItem = c_Items.begin(); c_ItItem != c_Items.end(); ++c_ItItem)
   {
      if ((*c_ItItem)->type() == msn_GRAPHICS_ITEM_NODE)
      {
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         C_GiNode * const pc_Node = dynamic_cast<C_GiNode * const>(*c_ItItem);
         if (pc_Node != NULL)
         {
            pc_Node->CheckNodeForChanges();
         }
      }

      if (((*c_ItItem)->type() == msn_GRAPHICS_ITEM_BUS) ||
          ((*c_ItItem)->type() == msn_GRAPHICS_ITEM_CANBUS) ||
          ((*c_ItItem)->type() == msn_GRAPHICS_ITEM_ETHERNETBUS))
      {
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         C_GiLiBus * const pc_Bus = dynamic_cast<C_GiLiBus * const>(*c_ItItem);
         if (pc_Bus != NULL)
         {
            pc_Bus->CheckBusForChanges();
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SebTopologyBaseScene::m_LoadProject(void)
{
   QVector<uint32> c_SaveNodeIndices;
   QVector<uint32> c_SaveBusIndices;
   QVector<uint32> c_SaveIndices;

   for (uint32 u32_It = 0; u32_It < C_PuiSdHandler::h_GetInstance()->GetOSCNodesSize(); ++u32_It)
   {
      c_SaveNodeIndices.append(u32_It);
   }
   for (uint32 u32_It = 0; u32_It < C_PuiSdHandler::h_GetInstance()->GetOSCBusesSize(); ++u32_It)
   {
      c_SaveBusIndices.append(u32_It);
   }

   //Fill up with zeros
   c_SaveIndices.append(0);
   c_SaveIndices.append(0);
   c_SaveIndices.append(0);
   c_SaveIndices.append(0);
   c_SaveIndices.append(0);

   this->m_LoadSubset(c_SaveNodeIndices, c_SaveBusIndices, c_SaveIndices, false, NULL, NULL);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load subset of system definition entries

   \param[in] orc_NodeIndices              Node indices to add
   \param[in] orc_BusIndices               Bus indices to add
   \param[in] orc_OtherStartIndices        Start indices
                                           0: Boundary
                                           1: Text element
                                           2: Line arrow
                                           3: Image
                                           4: Bus text element
   \param[in] orq_Selection                False: Ignore selection
   \param[in] opc_AdditionalConnectionData Additional data for bus connections
   \param[in] opc_IDMap                    Optional map for IDs to use
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebTopologyBaseScene::m_LoadSubset(const QVector<uint32> & orc_NodeIndices,
                                          const QVector<uint32> & orc_BusIndices,
                                          const QVector<uint32> & orc_OtherStartIndices, const bool & orq_Selection,
                                          const std::vector<C_PuiSdCompleteBusConnectionData> * const opc_AdditionalConnectionData,
                                          const QMap<C_PuiBsTemporaryDataID, uint64> * const opc_IDMap)
{
   //Load
   uint64 u64_CurUniqueID;
   uint32 u32_CurIndex;
   C_GiNode * pc_Node;

   std::vector<C_GiLiBus *> c_GraphicsBuses;
   C_GiLiBus * pc_BusReferenced;
   const C_OSCNode * pc_OSCNodeData;
   C_GiLiBus * pc_Bus;
   uint32 u32_BusConnectorCounter = 0;

   if (orq_Selection == true)
   {
      this->clearSelection();
   }

   //Create
   //Buses (first!)
   c_GraphicsBuses.resize(C_PuiSdHandler::h_GetInstance()->GetOSCBusesSize(), NULL);
   for (sint32 s32_ItBus = 0; s32_ItBus < orc_BusIndices.size(); ++s32_ItBus)
   {
      const C_OSCSystemBus * pc_OSCBusData;
      //ID
      u64_CurUniqueID =
         m_GetNewUniqueID(opc_IDMap, static_cast<sint32>(C_PuiSdDataElement::eBUS),
                          static_cast<uint32>(s32_ItBus));
      //Index
      u32_CurIndex = orc_BusIndices[s32_ItBus];
      //Data
      pc_OSCBusData = C_PuiSdHandler::h_GetInstance()->GetOSCBus(u32_CurIndex);
      //Create
      if (pc_OSCBusData != NULL)
      {
         uint32 u32_BusTextItem;
         C_GiTextElementBus * pc_BusTextItem = NULL;

         // search the matching bus text element
         for (u32_BusTextItem = orc_OtherStartIndices[4];
              u32_BusTextItem < C_PuiSdHandler::h_GetInstance()->c_BusTextElements.size();
              ++u32_BusTextItem)
         {
            if (C_PuiSdHandler::h_GetInstance()->c_BusTextElements[u32_BusTextItem].u32_BusIndex == u32_CurIndex)
            {
               // Item found, create it for the bus
               //ID
               const uint64 u64_BusTextUniqueID = m_GetNewUniqueID(opc_IDMap,
                                                                   static_cast<sint32>(C_PuiSdDataElement::
                                                                                       eTEXT_ELEMENT_BUS),
                                                                   u32_BusTextItem - orc_OtherStartIndices[4]);

               pc_BusTextItem = this->m_CreateBusTextElement(u32_BusTextItem, u64_BusTextUniqueID, NULL);
               pc_BusTextItem->LoadData();

               this->m_AddBusTextElementToScene(pc_BusTextItem);
               if (orq_Selection == true)
               {
                  m_UpdateSelection(pc_BusTextItem, false);
               }

               break;
            }
         }

         // create the bus
         if (pc_OSCBusData->e_Type == C_OSCSystemBus::E_Type::eETHERNET)
         {
            c_GraphicsBuses[u32_CurIndex] = m_CreateEthernetBus(u32_CurIndex, u64_CurUniqueID, pc_BusTextItem, NULL,
                                                                NULL);
         }
         else
         {
            c_GraphicsBuses[u32_CurIndex] = m_CreateCANBus(u32_CurIndex, u64_CurUniqueID, pc_BusTextItem, NULL, NULL);
         }
         m_AddBusToScene(c_GraphicsBuses[u32_CurIndex]);
         if (orq_Selection == true)
         {
            m_UpdateSelection(c_GraphicsBuses[u32_CurIndex], false);
         }
         Q_EMIT this->SigBusChanged(u32_CurIndex);
      }
   }
   //Nodes
   for (sint32 s32_ItNode = 0; s32_ItNode < orc_NodeIndices.size();
        ++s32_ItNode)
   {
      const C_PuiSdNode * pc_UINodeData;
      //ID
      u64_CurUniqueID = m_GetNewUniqueID(opc_IDMap, static_cast<sint32>(C_PuiSdDataElement::eNODE),
                                         static_cast<uint32>(s32_ItNode));
      //Index
      u32_CurIndex = orc_NodeIndices[s32_ItNode];
      //Data
      pc_UINodeData = C_PuiSdHandler::h_GetInstance()->GetUINode(u32_CurIndex);
      pc_OSCNodeData = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(u32_CurIndex);
      if ((pc_UINodeData != NULL) && (pc_OSCNodeData != NULL))
      {
         std::vector<C_PuiSdNodeConnection> c_UIBusConnections = pc_UINodeData->c_UIBusConnections;
         //create
         pc_Node = this->m_CreateNode(u32_CurIndex, u64_CurUniqueID, pc_UINodeData->f64_Width,
                                      pc_UINodeData->f64_Height,
                                      NULL);
         pc_Node->LoadData();

         m_AddNodeToScene(pc_Node);
         if (orq_Selection == true)
         {
            m_UpdateSelection(pc_Node, false);
         }
         Q_EMIT this->SigNodeChanged(u32_CurIndex);

         //Connectors
         for (uint32 u32_ItConnector = 0; u32_ItConnector < c_UIBusConnections.size();
              ++u32_ItConnector)
         {
            uint32 u32_BusIndex = 0;
            //Get bus name
            for (uint32 u32_ItComInt = 0; u32_ItComInt < pc_OSCNodeData->c_Properties.c_ComInterfaces.size();
                 ++u32_ItComInt)
            {
               const C_OSCNodeComInterfaceSettings & rc_ComInt =
                  pc_OSCNodeData->c_Properties.c_ComInterfaces[u32_ItComInt];
               if (rc_ComInt.q_IsBusConnected == true)
               {
                  if ((rc_ComInt.e_InterfaceType ==
                       pc_UINodeData->c_UIBusConnections[u32_ItConnector].c_ConnectionID.e_InterfaceType) &&
                      (rc_ComInt.u8_InterfaceNumber ==
                       pc_UINodeData->c_UIBusConnections[u32_ItConnector].c_ConnectionID.u8_InterfaceNumber))
                  {
                     u32_BusIndex = rc_ComInt.u32_BusIndex;
                  }
               }
            }
            //Look for bus graphics item
            pc_BusReferenced = NULL;
            for (uint32 u32_ItBus = 0; u32_ItBus < c_GraphicsBuses.size(); ++u32_ItBus)
            {
               pc_Bus = c_GraphicsBuses[u32_ItBus];
               if (pc_Bus != NULL)
               {
                  if (static_cast<uint32>(pc_Bus->GetIndex()) == u32_BusIndex)
                  {
                     pc_BusReferenced = c_GraphicsBuses[u32_ItBus];
                  }
               }
            }
            if ((pc_BusReferenced != NULL) &&
                (pc_UINodeData->c_UIBusConnections[u32_ItConnector].c_UINodeConnectionInteractionPoints.size() >
                 0))
            {
               C_GiLiBusConnector * pc_GraphicsConnector;

               //ID
               u64_CurUniqueID = m_GetNewUniqueBusConnectorID(opc_IDMap, u32_BusConnectorCounter,
                                                              opc_AdditionalConnectionData);
               ++u32_BusConnectorCounter;
               //Create
               pc_GraphicsConnector = new C_GiLiBusConnector(
                  u64_CurUniqueID, pc_UINodeData->c_UIBusConnections[
                     u32_ItConnector].c_UINodeConnectionInteractionPoints,
                  pc_Node,
                  pc_BusReferenced);

               pc_GraphicsConnector->RestoreZOrder();
               pc_Node->AddConnection(pc_GraphicsConnector);
               m_AddBusConnectorToScene(pc_GraphicsConnector);
            }
         }
      }
   }

   //Additional connectors
   if (opc_AdditionalConnectionData != NULL)
   {
      for (uint32 u32_ItConnector = 0; u32_ItConnector < opc_AdditionalConnectionData->size();
           ++u32_ItConnector)
      {
         C_PuiSdCompleteBusConnectionData c_CompleteBusConnectionData =
            (*opc_AdditionalConnectionData)[u32_ItConnector];
         //ID
         u64_CurUniqueID = m_GetNewUniqueID(opc_IDMap, static_cast<sint32>(C_PuiSdDataElement::eBUS_CONNECTOR),
                                            u32_ItConnector);
         //Look for bus graphics item
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         pc_BusReferenced = dynamic_cast<C_GiLiBus *>(this->GetItemByID(c_CompleteBusConnectionData.u64_BusID));
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         pc_Node = dynamic_cast<C_GiNode *>(this->GetItemByID(c_CompleteBusConnectionData.u64_NodeID));
         if ((pc_Node != NULL) && ((pc_BusReferenced != NULL) &&
                                   (c_CompleteBusConnectionData.c_UIData.c_UINodeConnectionInteractionPoints.size() >
                                    0)))
         {
            if (pc_Node->CheckInterfaceAvailable(pc_BusReferenced->GetType(),
                                                 c_CompleteBusConnectionData.c_ConnectionId.u8_InterfaceNumber) ==
                true)
            {
               const uint32 u32_NodeIndex = static_cast<uint32>(pc_Node->GetIndex());
               const C_OSCNode * const pc_NodeData = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(u32_NodeIndex);
               if (pc_NodeData != NULL)
               {
                  if (c_CompleteBusConnectionData.c_ConnectionId.u8_InterfaceNumber <
                      pc_NodeData->c_Properties.c_ComInterfaces.size())
                  {
                     const C_OSCNodeComInterfaceSettings * const pc_ComInterface =
                        pc_NodeData->c_Properties.GetComInterface(
                           pc_BusReferenced->GetType(), c_CompleteBusConnectionData.c_ConnectionId.
                           u8_InterfaceNumber);
                     if (pc_ComInterface != NULL)
                     {
                        C_GiLiBusConnector * pc_GraphicsConnector = new C_GiLiBusConnector(
                           u64_CurUniqueID, c_CompleteBusConnectionData.c_UIData.c_UINodeConnectionInteractionPoints,
                           pc_Node, pc_BusReferenced);

                        //Add to system definition data
                        C_PuiSdHandler::h_GetInstance()->AddConnection(
                           u32_NodeIndex, c_CompleteBusConnectionData.c_ConnectionId.u8_InterfaceNumber,
                           pc_ComInterface->u8_NodeID, static_cast<uint32>(pc_BusReferenced->GetIndex()));
                        pc_GraphicsConnector->RestoreZOrder();
                        pc_Node->AddConnection(pc_GraphicsConnector);
                        m_AddBusConnectorToScene(pc_GraphicsConnector);
                     }
                  }
               }
            }
         }
      }
   }

   // Boundary
   for (uint32 u32_Item = orc_OtherStartIndices[0];
        u32_Item < C_PuiSdHandler::h_GetInstance()->c_Elements.c_Boundaries.size();
        ++u32_Item)
   {
      C_PuiBsBoundary * pc_UIBoundaryData = &C_PuiSdHandler::h_GetInstance()->c_Elements.c_Boundaries[u32_Item];
      C_GiSdBoundary * pc_Item;

      //ID
      u64_CurUniqueID = m_GetNewUniqueID(opc_IDMap, static_cast<sint32>(C_PuiSdDataElement::eBOUNDARY),
                                         u32_Item - orc_OtherStartIndices[0]);

      pc_Item = new C_GiSdBoundary(u32_Item, u64_CurUniqueID,
                                   pc_UIBoundaryData->f64_Width, pc_UIBoundaryData->f64_Height);
      pc_Item->LoadData();

      this->m_AddBoundaryToScene(pc_Item);
      if (orq_Selection == true)
      {
         m_UpdateSelection(pc_Item, false);
      }
   }

   // Text Element
   for (uint32 u32_Item = orc_OtherStartIndices[1];
        u32_Item < C_PuiSdHandler::h_GetInstance()->c_Elements.c_TextElements.size();
        ++u32_Item)
   {
      C_GiSdTextElement * pc_Item;

      //ID
      u64_CurUniqueID = m_GetNewUniqueID(opc_IDMap, static_cast<sint32>(C_PuiSdDataElement::eTEXT_ELEMENT),
                                         u32_Item - orc_OtherStartIndices[1]);

      pc_Item = new C_GiSdTextElement(u32_Item, u64_CurUniqueID);
      pc_Item->LoadData();

      this->m_AddTextElementToScene(pc_Item);
      if (orq_Selection == true)
      {
         m_UpdateSelection(pc_Item, false);
      }
   }

   // Line arrow
   for (uint32 u32_Item = orc_OtherStartIndices[2];
        u32_Item < C_PuiSdHandler::h_GetInstance()->c_Elements.c_LineArrows.size();
        ++u32_Item)
   {
      C_GiSdArrow * pc_Item;

      //ID
      u64_CurUniqueID = m_GetNewUniqueID(opc_IDMap, static_cast<sint32>(C_PuiSdDataElement::eLINE_ARROW),
                                         u32_Item - orc_OtherStartIndices[2]);

      pc_Item = new C_GiSdArrow(u32_Item, u64_CurUniqueID);
      this->m_AddLineArrowToScene(pc_Item);
      if (orq_Selection == true)
      {
         m_UpdateSelection(pc_Item, false);
      }
   }

   // Image
   for (uint32 u32_Item = orc_OtherStartIndices[3];
        u32_Item < C_PuiSdHandler::h_GetInstance()->c_Elements.c_Images.size();
        ++u32_Item)
   {
      C_PuiBsImage * pc_UIImageData = &C_PuiSdHandler::h_GetInstance()->c_Elements.c_Images[u32_Item];
      C_GiSdImageGroup * pc_Item;

      //ID
      u64_CurUniqueID = m_GetNewUniqueID(opc_IDMap, static_cast<sint32>(C_PuiSdDataElement::eIMAGE),
                                         u32_Item - orc_OtherStartIndices[3]);

      pc_Item = new C_GiSdImageGroup(u32_Item,
                                     u64_CurUniqueID, pc_UIImageData->f64_Width,
                                     pc_UIImageData->f64_Height, pc_UIImageData->c_UIImagePixmap);
      pc_Item->LoadData();

      this->m_AddImageGroupToScene(pc_Item);
      if (orq_Selection == true)
      {
         m_UpdateSelection(pc_Item, false);
      }
   }

   //Clean selections of constructor
   if (orq_Selection == false)
   {
      this->clearSelection();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get new unique ID

   \param[in] opc_IDMap                    Optional map for item to id
   \param[in] oru32_Index                  Optional current index
   \param[in] opc_AdditionalConnectionData Optional connection data to orientate

   \return
   New unique ID
*/
//----------------------------------------------------------------------------------------------------------------------
uint64 C_SebTopologyBaseScene::m_GetNewUniqueBusConnectorID(const QMap<C_PuiBsTemporaryDataID,
                                                                       uint64> * const opc_IDMap,
                                                            const uint32 & oru32_Index,
                                                            const std::vector<C_PuiSdCompleteBusConnectionData> * const opc_AdditionalConnectionData)
{
   uint64 u64_Retval;

   //Check if there is a map
   if (opc_IDMap != NULL)
   {
      uint32 u32_Offset;

      //Offset for standard content of Bus Connector part of map
      if (opc_AdditionalConnectionData != NULL)
      {
         u32_Offset = opc_AdditionalConnectionData->size();
      }
      else
      {
         u32_Offset = 0;
      }

      //Get mapped ID
      u64_Retval =
         opc_IDMap->value(C_PuiBsTemporaryDataID(static_cast<sint32>(C_PuiSdDataElement::eBUS_CONNECTOR),
                                                 oru32_Index + u32_Offset), this->mu64_LastUnusedUniqueID);
      if (u64_Retval == this->mu64_LastUnusedUniqueID)
      {
         ++mu64_LastUnusedUniqueID;
      }
   }
   else
   {
      u64_Retval = this->mu64_LastUnusedUniqueID;
      ++mu64_LastUnusedUniqueID;
   }
   return u64_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add new bus text element to scene and connect signals

   \param[in,out] opc_Item Pointer to new bus text element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebTopologyBaseScene::m_AddBusTextElementToScene(C_GiTextElementBus * const opc_Item)
{
   this->m_AddTextElementToScene(opc_Item);
   connect(opc_Item, &C_GiTextElementBus::SigHideToolTip, this, &C_SebTopologyBaseScene::m_HandleHideToolTip);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add new node to scene and connect signals

   \param[in,out] opc_NodeGraphicsItem Pointer to new node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebTopologyBaseScene::m_AddNodeToScene(C_GiNode * const opc_NodeGraphicsItem)
{
   connect(opc_NodeGraphicsItem, &C_GiNode::SigHideToolTip, this,
           &C_SebTopologyBaseScene::m_HandleHideToolTip);

   // Context menu button of node
   m_AddRectBaseGroupToScene(opc_NodeGraphicsItem);

   opc_NodeGraphicsItem->GenerateHint();
   m_AddAnyItemToScene(opc_NodeGraphicsItem);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add new bus to scene and connect signals

   \param[in,out] opc_BusGraphicsItem Pointer to new bus
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebTopologyBaseScene::m_AddBusToScene(C_GiLiBus * const opc_BusGraphicsItem)
{
   connect(opc_BusGraphicsItem, &C_GiLiBus::SigHideToolTip, this,
           &C_SebTopologyBaseScene::m_HandleHideToolTip);

   m_AddLineGroupToScene(opc_BusGraphicsItem);

   opc_BusGraphicsItem->GenerateHint();
   m_AddAnyItemToScene(opc_BusGraphicsItem);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add new bus connector to scene and connect signals

   \param[in,out] opc_BusConnectorGraphicsItem Connector item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebTopologyBaseScene::m_AddBusConnectorToScene(C_GiLiBusConnector * const opc_BusConnectorGraphicsItem)
{
   if (opc_BusConnectorGraphicsItem != NULL)
   {
      // Connections
      connect(opc_BusConnectorGraphicsItem, &C_GiLiBusConnector::SigHintUpdate, this,
              &C_SebTopologyBaseScene::m_UpdateHints);
      m_AddBusConnectorBaseToScene(opc_BusConnectorGraphicsItem);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add new bus connector base item to scene and connect signals

   \param[in,out] opc_BusConnectorGraphicsItem Connector item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebTopologyBaseScene::m_AddBusConnectorBaseToScene(C_GiLiBusConnectorBase * const opc_BusConnectorGraphicsItem)
{
   if (opc_BusConnectorGraphicsItem != NULL)
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_SebUnoTopBaseManager * const pc_UndoManager = dynamic_cast<C_SebUnoTopBaseManager *>(this->m_GetUndoManager());
      // Connections
      connect(opc_BusConnectorGraphicsItem, &C_GiLiBusConnector::SigHideToolTip, this,
              &C_SebTopologyBaseScene::m_HandleHideToolTip);
      connect(opc_BusConnectorGraphicsItem, &C_GiLiBusConnector::SigShowToolTip, this,
              &C_SebTopologyBaseScene::m_StartTimerForToolTip);

      if (pc_UndoManager != NULL)
      {
         //Undo
         connect(opc_BusConnectorGraphicsItem, &C_GiLiBusConnector::SigPrepareMove, pc_UndoManager,
                 &C_SebUnoTopBaseManager::SaveBusConnectorInitialPoints);
         m_AddLineGroupToScene(opc_BusConnectorGraphicsItem);
      }
      //Item
      opc_BusConnectorGraphicsItem->GenerateHint();
      m_AddAnyItemToScene(opc_BusConnectorGraphicsItem);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize scene undo manager connections
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebTopologyBaseScene::m_InitSceneUndoManager(void)
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   C_SebUnoTopBaseManager * const pc_UndoManager = dynamic_cast<C_SebUnoTopBaseManager *>(this->m_GetUndoManager());

   if (pc_UndoManager != NULL)
   {
      // scene changed
      connect(pc_UndoManager, &C_SebUnoBaseManager::SigChanged, this, &C_SebTopologyBaseScene::SigChanged);
      connect(pc_UndoManager, &C_SebUnoBaseManager::SigErrorChanged, this,
              &C_SebTopologyBaseScene::CheckAllItemsForChanges);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Restore default cursors for all items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebTopologyBaseScene::m_RestoreCursors(void) const
{
   const QList<QGraphicsItem *> & rc_Items = this->items();

   for (QList<QGraphicsItem *>::const_iterator c_ItItem = rc_Items.begin(); c_ItItem != rc_Items.end(); ++c_ItItem)
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      //lint -e{740}  no problem because of common base class
      C_GiBiCustomMouseItem * const pc_Item = dynamic_cast<C_GiBiCustomMouseItem *>(*c_ItItem);
      if (pc_Item != NULL)
      {
         pc_Item->RestoreDefaultCursor();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten mouse move event slot

   Here: Signal bus text mouse move update

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebTopologyBaseScene::mouseMoveEvent(QGraphicsSceneMouseEvent * const opc_Event)
{
   QGraphicsItem * const pc_Item = this->itemAt(opc_Event->scenePos(), QTransform());
   QGraphicsItem * const pc_ItemParent = C_SebUtil::h_GetHighestParent(pc_Item);
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   C_GiTextElementBus * const pc_BusText = dynamic_cast<C_GiTextElementBus * const>(pc_ItemParent);

   if (pc_BusText != NULL)
   {
      pc_BusText->HandleMouseMoveToolTip(opc_Event->scenePos());
   }
   C_SebScene::mouseMoveEvent(opc_Event);
}
