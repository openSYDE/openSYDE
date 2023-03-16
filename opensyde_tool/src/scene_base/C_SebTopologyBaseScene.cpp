//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Common graphics scene with topology functionality (implementation)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QApplication>

#include "gitypes.hpp"
#include "TglUtils.hpp"

#include "C_SebTopologyBaseScene.hpp"

#include "C_GiLiLineGroup.hpp"
#include "C_GiArrowCursorButton.hpp"
#include "C_GiLiBus.hpp"
#include "C_GiLiEthernetBus.hpp"
#include "C_GiLiCanBus.hpp"

#include "C_PuiSdHandler.hpp"
#include "C_OscNode.hpp"
#include "C_OscSystemBus.hpp"

#include "C_SebUtil.hpp"
#include "C_PuiSdUtil.hpp"
#include "C_SebUnoTopBaseManager.hpp"
#include "C_GiSdBoundary.hpp"
#include "C_GiSdArrow.hpp"
#include "C_GiSdTextElement.hpp"
#include "C_GiSdImageGroup.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   \param[in,out]  opc_Parent    Optional pointer to parent
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
      if ((*c_ItItem)->type() == ms32_GRAPHICS_ITEM_NODE)
      {
         C_GiNode * const pc_Node = dynamic_cast<C_GiNode * const>(*c_ItItem);
         if (pc_Node != NULL)
         {
            pc_Node->CheckNodeForChanges();
         }
      }

      if (((*c_ItItem)->type() == ms32_GRAPHICS_ITEM_BUS) ||
          ((*c_ItItem)->type() == ms32_GRAPHICS_ITEM_CANBUS) ||
          ((*c_ItItem)->type() == ms32_GRAPHICS_ITEM_ETHERNETBUS))
      {
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
   QVector<uint32_t> c_SaveNodeIndices;
   QVector<uint32_t> c_SaveBusIndices;
   QVector<uint32_t> c_SaveIndices;

   for (uint32_t u32_It = 0; u32_It < C_PuiSdHandler::h_GetInstance()->GetOscNodesSize(); ++u32_It)
   {
      const bool q_IsFirst = C_PuiSdUtil::h_CheckIsFirstInAnyGroupOrNotInAny(u32_It,
                                                                             C_PuiSdHandler::h_GetInstance()->GetOscSystemDefinitionConst().c_NodeSquads);
      if (q_IsFirst)
      {
         c_SaveNodeIndices.append(u32_It);
      }
   }
   for (uint32_t u32_It = 0; u32_It < C_PuiSdHandler::h_GetInstance()->GetOscBusesSize(); ++u32_It)
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

   \param[in]  orc_NodeIndices               Node indices to add
   \param[in]  orc_BusIndices                Bus indices to add
   \param[in]  orc_OtherStartIndices         Start indices
                                             0: Boundary
                                             1: Text element
                                             2: Line arrow
                                             3: Image
                                             4: Bus text element
   \param[in]  orq_Selection                 False: Ignore selection
   \param[in]  opc_AdditionalConnectionData  Additional data for bus connections
   \param[in]  opc_IdMap                     Optional map for IDs to use
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebTopologyBaseScene::m_LoadSubset(const QVector<uint32_t> & orc_NodeIndices,
                                          const QVector<uint32_t> & orc_BusIndices,
                                          const QVector<uint32_t> & orc_OtherStartIndices, const bool & orq_Selection,
                                          const std::vector<C_PuiSdCompleteBusConnectionData> * const opc_AdditionalConnectionData,
                                          const QMap<C_PuiBsTemporaryDataId, uint64_t> * const opc_IdMap)
{
   //Load
   uint64_t u64_CurUniqueId;
   uint32_t u32_CurIndex;
   C_GiNode * pc_Node;

   std::vector<C_GiLiBus *> c_GraphicsBuses;
   C_GiLiBus * pc_BusReferenced;
   const C_OscNode * pc_OscNodeData;
   C_GiLiBus * pc_Bus;
   uint32_t u32_BusConnectorCounter = 0;

   if (orq_Selection == true)
   {
      this->clearSelection();
   }

   //Create
   //Buses (first!)
   c_GraphicsBuses.resize(C_PuiSdHandler::h_GetInstance()->GetOscBusesSize(), NULL);
   for (int32_t s32_ItBus = 0; s32_ItBus < orc_BusIndices.size(); ++s32_ItBus)
   {
      const C_OscSystemBus * pc_OscBusData;
      //ID
      u64_CurUniqueId =
         m_GetNewUniqueId(opc_IdMap, static_cast<int32_t>(C_PuiSdDataElement::eBUS),
                          static_cast<uint32_t>(s32_ItBus), opc_IdMap != NULL);
      //Index
      u32_CurIndex = orc_BusIndices[s32_ItBus];
      //Data
      pc_OscBusData = C_PuiSdHandler::h_GetInstance()->GetOscBus(u32_CurIndex);
      //Create
      if (pc_OscBusData != NULL)
      {
         uint32_t u32_BusTextItem;
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
               const uint64_t u64_BusTextUniqueId = m_GetNewUniqueId(opc_IdMap,
                                                                     static_cast<int32_t>(C_PuiSdDataElement::
                                                                                          eTEXT_ELEMENT_BUS),
                                                                     u32_BusTextItem - orc_OtherStartIndices[4],
                                                                     opc_IdMap != NULL);

               pc_BusTextItem = this->m_CreateBusTextElement(u32_BusTextItem, u64_BusTextUniqueId, NULL);
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
         if (pc_OscBusData->e_Type == C_OscSystemBus::E_Type::eETHERNET)
         {
            c_GraphicsBuses[u32_CurIndex] = m_CreateEthernetBus(u32_CurIndex, u64_CurUniqueId, pc_BusTextItem, NULL,
                                                                NULL);
         }
         else
         {
            c_GraphicsBuses[u32_CurIndex] = m_CreateCanBus(u32_CurIndex, u64_CurUniqueId, pc_BusTextItem, NULL, NULL);
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
   for (int32_t s32_ItNode = 0; s32_ItNode < orc_NodeIndices.size();
        ++s32_ItNode)
   {
      //Index
      u32_CurIndex = orc_NodeIndices[s32_ItNode];
      if (C_PuiSdUtil::h_CheckIsFirstInAnyGroupOrNotInAny(u32_CurIndex,
                                                          C_PuiSdHandler::h_GetInstance()->GetOscSystemDefinitionConst()
                                                          .
                                                          c_NodeSquads))
      {
         const C_PuiSdNode * pc_UiNodeData;
         //ID
         u64_CurUniqueId = m_GetNewUniqueId(opc_IdMap, static_cast<int32_t>(C_PuiSdDataElement::eNODE),
                                            static_cast<uint32_t>(s32_ItNode), opc_IdMap != NULL);
         //Data
         pc_UiNodeData = C_PuiSdHandler::h_GetInstance()->GetUiNode(u32_CurIndex);
         pc_OscNodeData = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(u32_CurIndex);
         if ((pc_UiNodeData != NULL) && (pc_OscNodeData != NULL))
         {
            const std::vector<C_PuiSdNodeConnection> c_UiBusConnections = pc_UiNodeData->c_UiBusConnections;
            //create
            pc_Node = this->m_CreateNode(u32_CurIndex, u64_CurUniqueId, pc_UiNodeData->f64_Width,
                                         pc_UiNodeData->f64_Height,
                                         NULL);
            pc_Node->LoadData();

            m_AddNodeToScene(pc_Node);
            if (orq_Selection == true)
            {
               m_UpdateSelection(pc_Node, false);
            }
            Q_EMIT this->SigNodeChanged(u32_CurIndex);
            //Connectors
            for (uint32_t u32_ItConnector = 0; u32_ItConnector < c_UiBusConnections.size();
                 ++u32_ItConnector)
            {
               uint32_t u32_BusIndex = 0;
               //Get bus name
               for (uint32_t u32_ItComInt = 0; u32_ItComInt < pc_OscNodeData->c_Properties.c_ComInterfaces.size();
                    ++u32_ItComInt)
               {
                  const C_OscNodeComInterfaceSettings & rc_ComInt =
                     pc_OscNodeData->c_Properties.c_ComInterfaces[u32_ItComInt];
                  if (rc_ComInt.GetBusConnectedRawValue() == true)
                  {
                     if ((rc_ComInt.e_InterfaceType ==
                          pc_UiNodeData->c_UiBusConnections[u32_ItConnector].c_ConnectionId.e_InterfaceType) &&
                         (rc_ComInt.u8_InterfaceNumber ==
                          pc_UiNodeData->c_UiBusConnections[u32_ItConnector].c_ConnectionId.u8_InterfaceNumber))
                     {
                        u32_BusIndex = rc_ComInt.u32_BusIndex;
                     }
                  }
               }
               //Look for bus graphics item
               pc_BusReferenced = NULL;
               for (uint32_t u32_ItBus = 0; u32_ItBus < c_GraphicsBuses.size(); ++u32_ItBus)
               {
                  pc_Bus = c_GraphicsBuses[u32_ItBus];
                  if (pc_Bus != NULL)
                  {
                     if (static_cast<uint32_t>(pc_Bus->GetIndex()) == u32_BusIndex)
                     {
                        pc_BusReferenced = c_GraphicsBuses[u32_ItBus];
                     }
                  }
               }
               if ((pc_BusReferenced != NULL) &&
                   (pc_UiNodeData->c_UiBusConnections[u32_ItConnector].c_UiNodeConnectionInteractionPoints.size() >
                    0))
               {
                  C_GiLiBusConnector * pc_GraphicsConnector;

                  //ID
                  u64_CurUniqueId = m_GetNewUniqueBusConnectorId(opc_IdMap, u32_BusConnectorCounter,
                                                                 opc_AdditionalConnectionData);
                  ++u32_BusConnectorCounter;
                  //Create
                  pc_GraphicsConnector = new C_GiLiBusConnector(
                     u64_CurUniqueId, pc_UiNodeData->c_UiBusConnections[
                        u32_ItConnector].c_UiNodeConnectionInteractionPoints,
                     pc_Node,
                     pc_BusReferenced);

                  pc_GraphicsConnector->RestoreZetOrder();
                  pc_Node->AddConnection(pc_GraphicsConnector);
                  m_AddBusConnectorToScene(pc_GraphicsConnector);
               }
            }
         }
      }
   }

   //Additional connectors
   if (opc_AdditionalConnectionData != NULL)
   {
      for (uint32_t u32_ItConnector = 0; u32_ItConnector < opc_AdditionalConnectionData->size();
           ++u32_ItConnector)
      {
         const C_PuiSdCompleteBusConnectionData c_CompleteBusConnectionData =
            (*opc_AdditionalConnectionData)[u32_ItConnector];
         //ID
         u64_CurUniqueId = m_GetNewUniqueId(opc_IdMap, static_cast<int32_t>(C_PuiSdDataElement::eBUS_CONNECTOR),
                                            u32_ItConnector, opc_IdMap != NULL);
         //Look for bus graphics item
         pc_BusReferenced = dynamic_cast<C_GiLiBus *>(this->GetItemById(c_CompleteBusConnectionData.u64_BusId));
         pc_Node = dynamic_cast<C_GiNode *>(this->GetItemById(c_CompleteBusConnectionData.u64_NodeId));
         if ((pc_Node != NULL) && ((pc_BusReferenced != NULL) &&
                                   (c_CompleteBusConnectionData.c_UiData.c_UiNodeConnectionInteractionPoints.size() >
                                    0)))
         {
            if (pc_Node->CheckInterfaceAvailable(pc_BusReferenced->GetType(),
                                                 c_CompleteBusConnectionData.c_ConnectionId.u8_InterfaceNumber) ==
                true)
            {
               const uint32_t u32_NodeIndex = static_cast<uint32_t>(pc_Node->GetIndex());
               const std::vector<uint32_t> c_NodeIndices =
                  C_PuiSdHandler::h_GetInstance()->GetAllNodeGroupIndicesUsingNodeIndex(
                     u32_NodeIndex);
               std::vector<C_PuiSdNodeInterfaceAutomaticProperties> c_Properties;
               C_PuiSdUtil::h_GetInterfaceDataForNode(u32_NodeIndex, c_CompleteBusConnectionData.c_ConnectionId,
                                                      c_Properties);
               if (c_Properties.size() == c_NodeIndices.size())
               {
                  C_GiLiBusConnector * const pc_GraphicsConnector = new C_GiLiBusConnector(
                     u64_CurUniqueId, c_CompleteBusConnectionData.c_UiData.c_UiNodeConnectionInteractionPoints,
                     pc_Node, pc_BusReferenced);

                  //Add to system definition data
                  C_PuiSdHandler::h_GetInstance()->AddConnection(
                     u32_NodeIndex, c_CompleteBusConnectionData.c_ConnectionId.u8_InterfaceNumber,
                     c_Properties, static_cast<uint32_t>(pc_BusReferenced->GetIndex()));
                  pc_GraphicsConnector->RestoreZetOrder();
                  pc_Node->AddConnection(pc_GraphicsConnector);
                  m_AddBusConnectorToScene(pc_GraphicsConnector);
               }
            }
         }
      }
   }

   // Boundary
   for (uint32_t u32_Item = orc_OtherStartIndices[0];
        u32_Item < C_PuiSdHandler::h_GetInstance()->c_Elements.c_Boundaries.size();
        ++u32_Item)
   {
      C_PuiBsBoundary * const pc_UiBoundaryData = &C_PuiSdHandler::h_GetInstance()->c_Elements.c_Boundaries[u32_Item];
      C_GiSdBoundary * pc_Item;

      //ID
      u64_CurUniqueId = m_GetNewUniqueId(opc_IdMap, static_cast<int32_t>(C_PuiSdDataElement::eBOUNDARY),
                                         u32_Item - orc_OtherStartIndices[0], opc_IdMap != NULL);

      pc_Item = new C_GiSdBoundary(u32_Item, u64_CurUniqueId,
                                   pc_UiBoundaryData->f64_Width, pc_UiBoundaryData->f64_Height);
      pc_Item->LoadData();

      this->m_AddBoundaryToScene(pc_Item);
      if (orq_Selection == true)
      {
         m_UpdateSelection(pc_Item, false);
      }
   }

   // Text Element
   for (uint32_t u32_Item = orc_OtherStartIndices[1];
        u32_Item < C_PuiSdHandler::h_GetInstance()->c_Elements.c_TextElements.size();
        ++u32_Item)
   {
      C_GiSdTextElement * pc_Item;

      //ID
      u64_CurUniqueId = m_GetNewUniqueId(opc_IdMap, static_cast<int32_t>(C_PuiSdDataElement::eTEXT_ELEMENT),
                                         u32_Item - orc_OtherStartIndices[1], opc_IdMap != NULL);

      pc_Item = new C_GiSdTextElement(u32_Item, u64_CurUniqueId);
      pc_Item->LoadData();

      this->m_AddTextElementToScene(pc_Item);
      if (orq_Selection == true)
      {
         m_UpdateSelection(pc_Item, false);
      }
   }

   // Line arrow
   for (uint32_t u32_Item = orc_OtherStartIndices[2];
        u32_Item < C_PuiSdHandler::h_GetInstance()->c_Elements.c_LineArrows.size();
        ++u32_Item)
   {
      C_GiSdArrow * pc_Item;

      //ID
      u64_CurUniqueId = m_GetNewUniqueId(opc_IdMap, static_cast<int32_t>(C_PuiSdDataElement::eLINE_ARROW),
                                         u32_Item - orc_OtherStartIndices[2], opc_IdMap != NULL);

      pc_Item = new C_GiSdArrow(u32_Item, u64_CurUniqueId);
      this->m_AddLineArrowToScene(pc_Item);
      if (orq_Selection == true)
      {
         m_UpdateSelection(pc_Item, false);
      }
   }

   // Image
   for (uint32_t u32_Item = orc_OtherStartIndices[3];
        u32_Item < C_PuiSdHandler::h_GetInstance()->c_Elements.c_Images.size();
        ++u32_Item)
   {
      C_PuiBsImage * const pc_UiImageData = &C_PuiSdHandler::h_GetInstance()->c_Elements.c_Images[u32_Item];
      C_GiSdImageGroup * pc_Item;

      //ID
      u64_CurUniqueId = m_GetNewUniqueId(opc_IdMap, static_cast<int32_t>(C_PuiSdDataElement::eIMAGE),
                                         u32_Item - orc_OtherStartIndices[3], opc_IdMap != NULL);

      pc_Item = new C_GiSdImageGroup(u32_Item,
                                     u64_CurUniqueId, pc_UiImageData->f64_Width,
                                     pc_UiImageData->f64_Height, pc_UiImageData->c_UiImagePixmap);
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

   \param[in]  opc_IdMap                     Optional map for item to id
   \param[in]  oru32_Index                   Optional current index
   \param[in]  opc_AdditionalConnectionData  Optional connection data to orientate

   \return
   New unique ID
*/
//----------------------------------------------------------------------------------------------------------------------
uint64_t C_SebTopologyBaseScene::m_GetNewUniqueBusConnectorId(const QMap<C_PuiBsTemporaryDataId,
                                                                         uint64_t> * const opc_IdMap,
                                                              const uint32_t & oru32_Index,
                                                              const std::vector<C_PuiSdCompleteBusConnectionData> * const opc_AdditionalConnectionData)
{
   uint64_t u64_Retval;

   //Check if there is a map
   if (opc_IdMap != NULL)
   {
      uint32_t u32_Offset;

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
         opc_IdMap->value(C_PuiBsTemporaryDataId(static_cast<int32_t>(C_PuiSdDataElement::eBUS_CONNECTOR),
                                                 oru32_Index + u32_Offset), this->mu64_LastUnusedUniqueId);
      if (u64_Retval == this->mu64_LastUnusedUniqueId)
      {
         ++mu64_LastUnusedUniqueId;
      }
   }
   else
   {
      u64_Retval = this->mu64_LastUnusedUniqueId;
      ++mu64_LastUnusedUniqueId;
   }
   return u64_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add new bus text element to scene and connect signals

   \param[in,out]  opc_Item   Pointer to new bus text element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebTopologyBaseScene::m_AddBusTextElementToScene(C_GiTextElementBus * const opc_Item)
{
   this->m_AddTextElementToScene(opc_Item);
   connect(opc_Item, &C_GiTextElementBus::SigHideToolTip, this, &C_SebTopologyBaseScene::m_HandleHideToolTip);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add new node to scene and connect signals

   \param[in,out]  opc_NodeGraphicsItem   Pointer to new node
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

   \param[in,out]  opc_BusGraphicsItem    Pointer to new bus
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

   \param[in,out]  opc_BusConnectorGraphicsItem    Connector item
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

   \param[in,out]  opc_BusConnectorGraphicsItem    Connector item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebTopologyBaseScene::m_AddBusConnectorBaseToScene(C_GiLiBusConnectorBase * const opc_BusConnectorGraphicsItem)
{
   if (opc_BusConnectorGraphicsItem != NULL)
   {
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

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebTopologyBaseScene::mouseMoveEvent(QGraphicsSceneMouseEvent * const opc_Event)
{
   QGraphicsItem * const pc_Item = this->itemAt(opc_Event->scenePos(), QTransform());
   QGraphicsItem * const pc_ItemParent = C_SebUtil::h_GetHighestParent(pc_Item);
   C_GiTextElementBus * const pc_BusText = dynamic_cast<C_GiTextElementBus * const>(pc_ItemParent);

   if (pc_BusText != NULL)
   {
      pc_BusText->HandleMouseMoveToolTip(opc_Event->scenePos());
   }
   C_SebScene::mouseMoveEvent(opc_Event);
}
