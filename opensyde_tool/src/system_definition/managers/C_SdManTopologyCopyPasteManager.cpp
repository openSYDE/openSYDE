//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handle most parts of copy paste implementation for scene (implementation)

   Handle most parts of copy paste implementation for scene

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <limits>
#include "stwerrors.hpp"
#include "C_SdManTopologyCopyPasteManager.hpp"

#include "C_GiNode.hpp"
#include "C_GiLiBus.hpp"
#include "C_GiSdImageGroup.hpp"
#include "C_GiSdBoundary.hpp"
#include "C_GiSdArrow.hpp"
#include "C_GiSdTextElement.hpp"
#include "C_GiTextElementBus.hpp"
#include "C_SdClipBoardHelper.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_SdTopologyDataSnapshot.hpp"

#include "gitypes.hpp"
#include "C_SebUtil.hpp"
#include "TglUtils.hpp"
#include "C_PuiSdUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::tgl;

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
C_SdManTopologyCopyPasteManager::C_SdManTopologyCopyPasteManager(void) :
   C_SebBaseCopyPasteManager()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdManTopologyCopyPasteManager::~C_SdManTopologyCopyPasteManager(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get data snapshot

   \param[in,out]  opc_Parent    Optional pointer to parent (unused here)

   Warning: calling function has to delete the created object

   \return
   NULL No valid data snap shot found
   Else Valid data snapshot
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiBsElements * C_SdManTopologyCopyPasteManager::GetSnapshot(QWidget * const opc_Parent)
{
   Q_UNUSED(opc_Parent)
   return &this->mc_LastKnownData;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Copy selected files to copy paste manager

   \param[in]  orc_SelectedItems       Selected items to copy
   \param[in]  orc_NormalizedZetValues   Normalized Z values for all copied items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManTopologyCopyPasteManager::CopyFromSceneToManager(const QList<QGraphicsItem *> & orc_SelectedItems,
                                                             const QMap<const QGraphicsItem *,
                                                                        float64_t> & orc_NormalizedZetValues)
{
   C_SdTopologyDataSnapshot c_Snapshot;
   uint32_t u32_Index;
   const C_GiLiBus * pc_Bus;
   const C_GiSdImageGroup * pc_Image;
   const C_GiSdBoundary * pc_Boundary;
   const C_GiSdArrow * pc_LineArrow;
   const C_GiSdTextElement * pc_TextElement;
   const C_GiTextElementBus * pc_TextElementBus;

   QMap<uint32_t, uint32_t> c_MapOldIndexToNewIndex;

   std::vector<const QGraphicsItem *> c_HandledItems;

   //Handle Z order

   //Clear data
   for (QList<QGraphicsItem *>::const_iterator c_ItItem = orc_SelectedItems.begin();
        c_ItItem != orc_SelectedItems.end();
        ++c_ItItem)
   {
      bool q_Handled = false;
      const QGraphicsItem * const pc_CurItem = C_SebUtil::h_GetHighestParent(*c_ItItem);
      for (uint32_t u32_ItHandledItems = 0; u32_ItHandledItems < c_HandledItems.size(); ++u32_ItHandledItems)
      {
         if (c_HandledItems[u32_ItHandledItems] == pc_CurItem)
         {
            q_Handled = true;
         }
      }
      if (q_Handled == false)
      {
         // copy the element
         switch (pc_CurItem->type())
         {
         case ms32_GRAPHICS_ITEM_NODE:
            C_SdManTopologyCopyPasteManager::mh_CopyFromSceneToManagerHandleNode(dynamic_cast<const C_GiNode *>(
                                                                                    pc_CurItem),
                                                                                 c_Snapshot, orc_NormalizedZetValues,
                                                                                 *c_ItItem);
            break;
         case ms32_GRAPHICS_ITEM_CANBUS: //Same data element
         case ms32_GRAPHICS_ITEM_ETHERNETBUS:

            pc_Bus = dynamic_cast<const C_GiLiBus *>(pc_CurItem);
            if (pc_Bus != NULL)
            {
               u32_Index = static_cast<uint32_t>(pc_Bus->GetIndex());
               const C_OscSystemBus * const pc_OscBus = C_PuiSdHandler::h_GetInstance()->GetOscBus(u32_Index);
               const C_PuiSdBus * const pc_UiBus = C_PuiSdHandler::h_GetInstance()->GetUiBus(u32_Index);
               if ((pc_OscBus != NULL) && (pc_UiBus != NULL))
               {
                  c_MapOldIndexToNewIndex.insert(u32_Index, c_Snapshot.c_OscBuses.size());
                  c_Snapshot.c_OscBuses.push_back(*pc_OscBus);
                  c_Snapshot.c_UiBuses.push_back(*pc_UiBus);
                  C_SebBaseCopyPasteManager::mh_HandleZetValueLine(*c_ItItem, orc_NormalizedZetValues,
                                                                   c_Snapshot.c_UiBuses[static_cast<std::vector<C_PuiSdBus>
                                                                                                    ::size_type>(
                                                                                           c_Snapshot.
                                                                                           c_UiBuses
                                                                                           .size() -
                                                                                           1UL)]);
               }
            }
            break;
         case ms32_GRAPHICS_ITEM_TEXTELEMENT:

            pc_TextElement = dynamic_cast<const C_GiSdTextElement *>(pc_CurItem);
            if (pc_TextElement != NULL)
            {
               u32_Index = static_cast<uint32_t>(pc_TextElement->GetIndex());
               if (u32_Index < C_PuiSdHandler::h_GetInstance()->c_Elements.c_TextElements.size())
               {
                  c_Snapshot.c_TextElements.push_back(C_PuiSdHandler::h_GetInstance()->c_Elements.c_TextElements[
                                                         u32_Index]);
                  C_SebBaseCopyPasteManager::mh_HandleZetValueBox(*c_ItItem, orc_NormalizedZetValues,
                                                                  c_Snapshot.c_TextElements[static_cast<std::vector<C_PuiBsTextElement>
                                                                                                        ::size_type>(
                                                                                               c_Snapshot.c_TextElements
                                                                                               .
                                                                                               size()
                                                                                               -
                                                                                               1UL)]);
               }
            }
            break;
         case ms32_GRAPHICS_ITEM_TEXTELEMENT_BUS:

            pc_TextElementBus = dynamic_cast<const C_GiTextElementBus *>(pc_CurItem);
            if (pc_TextElementBus != NULL)
            {
               u32_Index = static_cast<uint32_t>(pc_TextElementBus->GetIndex());
               if (u32_Index < C_PuiSdHandler::h_GetInstance()->c_BusTextElements.size())
               {
                  c_Snapshot.c_BusTextElements.push_back(C_PuiSdHandler::h_GetInstance()->c_BusTextElements[u32_Index]);
                  C_SebBaseCopyPasteManager::mh_HandleZetValueBox(*c_ItItem, orc_NormalizedZetValues,
                                                                  c_Snapshot.c_BusTextElements[static_cast<std::vector<C_PuiSdTextElementBus>
                                                                                                           ::size_type>(
                                                                                                  c_Snapshot.
                                                                                                  c_BusTextElements
                                                                                                  .size()
                                                                                                  -
                                                                                                  1UL)]);
               }
            }
            break;
         case ms32_GRAPHICS_ITEM_BOUNDARY:

            pc_Boundary = dynamic_cast<const C_GiSdBoundary *>(pc_CurItem);
            if (pc_Boundary != NULL)
            {
               u32_Index = static_cast<uint32_t>(pc_Boundary->GetIndex());
               if (u32_Index < C_PuiSdHandler::h_GetInstance()->c_Elements.c_Boundaries.size())
               {
                  c_Snapshot.c_Boundaries.push_back(C_PuiSdHandler::h_GetInstance()->c_Elements.c_Boundaries[u32_Index]);
                  C_SebBaseCopyPasteManager::mh_HandleZetValueBox(*c_ItItem, orc_NormalizedZetValues,
                                                                  c_Snapshot.c_Boundaries[static_cast<std::vector<C_PuiBsBoundary>
                                                                                                      ::size_type>(
                                                                                             c_Snapshot.
                                                                                             c_Boundaries
                                                                                             .size() -
                                                                                             1UL)]);
               }
            }
            break;
         case ms32_GRAPHICS_ITEM_IMAGE:

            pc_Image = dynamic_cast<const C_GiSdImageGroup *>(pc_CurItem);
            if (pc_Image != NULL)
            {
               u32_Index = static_cast<uint32_t>(pc_Image->GetIndex());
               if (u32_Index < C_PuiSdHandler::h_GetInstance()->c_Elements.c_Images.size())
               {
                  c_Snapshot.c_Images.push_back(C_PuiSdHandler::h_GetInstance()->c_Elements.c_Images[u32_Index]);
                  C_SebBaseCopyPasteManager::mh_HandleZetValueBox(*c_ItItem, orc_NormalizedZetValues,
                                                                  c_Snapshot.c_Images[static_cast<std::vector<C_PuiBsImage>
                                                                                                  ::size_type>(
                                                                                         c_Snapshot.
                                                                                         c_Images.
                                                                                         size() -
                                                                                         1UL)]);
               }
            }
            break;
         case ms32_GRAPHICS_ITEM_LINE_ARROW:

            pc_LineArrow = dynamic_cast<const C_GiSdArrow *>(pc_CurItem);
            if (pc_LineArrow != NULL)
            {
               u32_Index = static_cast<uint32_t>(pc_LineArrow->GetIndex());
               if (u32_Index < C_PuiSdHandler::h_GetInstance()->c_Elements.c_LineArrows.size())
               {
                  c_Snapshot.c_LineArrows.push_back(C_PuiSdHandler::h_GetInstance()->c_Elements.c_LineArrows[u32_Index]);
                  C_SebBaseCopyPasteManager::mh_HandleZetValueLine(*c_ItItem, orc_NormalizedZetValues,
                                                                   c_Snapshot.c_LineArrows[static_cast<std::vector<C_PuiBsLineArrow>
                                                                                                       ::size_type>(
                                                                                              c_Snapshot.
                                                                                              c_LineArrows
                                                                                              .size() -
                                                                                              1UL)]);
               }
            }
            break;
         default:
            break;
         }
         c_HandledItems.push_back(pc_CurItem);
      }
   }
   //Adapt connections
   for (uint32_t u32_ItNode = 0; u32_ItNode < c_Snapshot.c_OscNodes.size(); ++u32_ItNode)
   {
      C_OscNode & rc_Node = c_Snapshot.c_OscNodes[u32_ItNode];
      for (uint32_t u32_ItInterface = 0; u32_ItInterface < rc_Node.c_Properties.c_ComInterfaces.size();
           ++u32_ItInterface)
      {
         C_OscNodeComInterfaceSettings & rc_ComInterface = rc_Node.c_Properties.c_ComInterfaces[u32_ItInterface];
         if (rc_ComInterface.GetBusConnectedRawValue() == true)
         {
            //Check if bus was copied
            //Replace
            rc_ComInterface.u32_BusIndex = c_MapOldIndexToNewIndex.value(rc_ComInterface.u32_BusIndex,
                                                                         std::numeric_limits<uint32_t>::max());
            if (rc_ComInterface.u32_BusIndex == std::numeric_limits<uint32_t>::max())
            {
               C_PuiSdNodeConnectionId c_Id;
               c_Id.u8_InterfaceNumber = rc_ComInterface.u8_InterfaceNumber;
               c_Id.e_InterfaceType = rc_ComInterface.e_InterfaceType;
               mh_RemoveConnection(c_Snapshot, u32_ItNode, c_Id);
            }
         }
      }
   }
   //Adapt bus text elements
   //lint -e{850} Index modified for erase index step
   for (uint32_t u32_ItBusTextElement = 0; u32_ItBusTextElement < c_Snapshot.c_BusTextElements.size();
        ++u32_ItBusTextElement)
   {
      C_PuiSdTextElementBus & rc_BusTextElement = c_Snapshot.c_BusTextElements[u32_ItBusTextElement];
      //Replace
      rc_BusTextElement.u32_BusIndex = c_MapOldIndexToNewIndex.value(rc_BusTextElement.u32_BusIndex,
                                                                     std::numeric_limits<uint32_t>::max());
      tgl_assert(rc_BusTextElement.u32_BusIndex != std::numeric_limits<uint32_t>::max());
      if (rc_BusTextElement.u32_BusIndex == std::numeric_limits<uint32_t>::max())
      {
         c_Snapshot.c_BusTextElements.erase(c_Snapshot.c_BusTextElements.begin() + u32_ItBusTextElement);
         //No iteration step as new element is at current position
         --u32_ItBusTextElement;
      }
   }
   //Handle CANopen
   C_SdManTopologyCopyPasteManager::mh_RemoveCanOpenInformation(c_Snapshot);
   //Handle position of copied items
   this->m_CalcOriginalPosition(&c_Snapshot);
   //reset paste counter
   this->mu32_PasteCounter = 0;

   //Copy to clip board
   C_SdClipBoardHelper::h_StoreDataSnapShotToClipboard(c_Snapshot);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Indicator if copy paste manager has some content

   Warning: is expected to be called before getting any data

   \return
   true: content
   false: no content
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdManTopologyCopyPasteManager::CheckValidContentAndPrepareData(void)
{
   this->mc_LastKnownData.Clear();
   return (C_SdClipBoardHelper::h_LoadDataSnapShotFromClipboard(this->mc_LastKnownData) == C_NO_ERR);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Evaluate all items to get top left point

   \param[in]  opc_Data    Data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManTopologyCopyPasteManager::m_CalcOriginalPosition(const C_PuiBsElements * const opc_Data)
{
   const C_SdTopologyDataSnapshot * const pc_ExpectedData =
      dynamic_cast<const C_SdTopologyDataSnapshot * const>(opc_Data);

   C_SebBaseCopyPasteManager::m_CalcOriginalPosition(opc_Data);
   if (pc_ExpectedData != NULL)
   {
      uint32_t u32_ItElem;

      for (u32_ItElem = 0; u32_ItElem < pc_ExpectedData->c_UiBuses.size(); ++u32_ItElem)
      {
         const C_PuiSdBus & rc_Bus = pc_ExpectedData->c_UiBuses[u32_ItElem];
         for (uint32_t u32_ItConnPoint = 0; u32_ItConnPoint < rc_Bus.c_UiInteractionPoints.size(); ++u32_ItConnPoint)
         {
            m_MinToOrgPos(rc_Bus.c_UiInteractionPoints[u32_ItConnPoint]);
         }
      }

      for (u32_ItElem = 0; u32_ItElem < pc_ExpectedData->c_UiNodes.size(); ++u32_ItElem)
      {
         const C_PuiSdNode & rc_Node = pc_ExpectedData->c_UiNodes[u32_ItElem];
         m_MinToOrgPos(rc_Node.c_UiPosition);

         for (uint32_t u32_ItConn = 0; u32_ItConn < rc_Node.c_UiBusConnections.size(); ++u32_ItConn)
         {
            for (uint32_t u32_ItConnPoint = 0;
                 u32_ItConnPoint < rc_Node.c_UiBusConnections[u32_ItConn].c_UiNodeConnectionInteractionPoints.size();
                 ++u32_ItConnPoint)
            {
               mh_Min(this->mc_OriginalPosition,
                      rc_Node.c_UiBusConnections[u32_ItConn].c_UiNodeConnectionInteractionPoints[u32_ItConnPoint]);
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Copy from scene to manager, handle node

   \param[in]      opc_UiNodeItem         UI node item
   \param[in,out]  orc_Snapshot           Snapshot
   \param[in]      orc_NormalizedZetValues  Normalized Z values
   \param[in]      opc_NodeItemOrigin     Node item origin
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManTopologyCopyPasteManager::mh_CopyFromSceneToManagerHandleNode(const C_GiNode * const opc_UiNodeItem,
                                                                          C_SdTopologyDataSnapshot & orc_Snapshot,
                                                                          const QMap<const QGraphicsItem *,
                                                                                     float64_t> & orc_NormalizedZetValues,
                                                                          const QGraphicsItem * const opc_NodeItemOrigin)
{
   if (opc_UiNodeItem != NULL)
   {
      const uint32_t u32_Index = static_cast<uint32_t>(opc_UiNodeItem->GetIndex());
      const bool q_IsMulti = C_OscNodeSquad::h_CheckIsMultiDevice(u32_Index,
                                                                  C_PuiSdHandler::h_GetInstance()->GetOscSystemDefinitionConst().c_NodeSquads);
      if (q_IsMulti)
      {
         C_SdManTopologyCopyPasteManager::mh_CopyFromSceneToManagerHandleMultiNode(u32_Index, orc_Snapshot,
                                                                                   orc_NormalizedZetValues,
                                                                                   opc_NodeItemOrigin);
      }
      else
      {
         const C_OscNode * const pc_OscNode = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(u32_Index);
         const C_PuiSdNode * const pc_UiNode = C_PuiSdHandler::h_GetInstance()->GetUiNode(u32_Index);
         if ((pc_OscNode != NULL) && (pc_UiNode != NULL))
         {
            orc_Snapshot.c_OscNodes.push_back(*pc_OscNode);
            orc_Snapshot.c_UiNodes.push_back(*pc_UiNode);
            C_SebBaseCopyPasteManager::mh_HandleZetValueBox(opc_NodeItemOrigin, orc_NormalizedZetValues,
                                                            orc_Snapshot.c_UiNodes[static_cast<std::vector<C_PuiSdNode>
                                                                                               ::size_type>(
                                                                                      orc_Snapshot.c_UiNodes.size()
                                                                                      -
                                                                                      1UL)]);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Copy from scene to manager, handle multi node

   \param[in]      ou32_NodeIndex         Node index
   \param[in,out]  orc_Snapshot           Snapshot
   \param[in]      orc_NormalizedZetValues  Normalized Z values
   \param[in]      opc_NodeItemOrigin     Node item origin
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManTopologyCopyPasteManager::mh_CopyFromSceneToManagerHandleMultiNode(const uint32_t ou32_NodeIndex,
                                                                               C_SdTopologyDataSnapshot & orc_Snapshot,
                                                                               const QMap<const QGraphicsItem *,
                                                                                          float64_t> & orc_NormalizedZetValues,
                                                                               const QGraphicsItem * const opc_NodeItemOrigin)
{
   const bool q_IsFirst = C_PuiSdUtil::h_CheckIsFirstInAnyGroupOrNotInAny(ou32_NodeIndex,
                                                                          C_PuiSdHandler::h_GetInstance()->GetOscSystemDefinitionConst().c_NodeSquads);

   if (q_IsFirst)
   {
      const std::vector<uint32_t> c_Indices =
         C_PuiSdHandler::h_GetInstance()->GetAllNodeGroupIndicesUsingNodeIndex(
            ou32_NodeIndex);
      uint32_t u32_GroupIndex;
      if (C_PuiSdHandler::h_GetInstance()->GetNodeSquadIndexWithNodeIndex(ou32_NodeIndex,
                                                                          u32_GroupIndex) == C_NO_ERR)
      {
         const C_OscNodeSquad * const pc_Group =
            C_PuiSdHandler::h_GetInstance()->GetOscNodeSquadConst(
               u32_GroupIndex);
         if (pc_Group != NULL)
         {
            C_OscNodeSquad c_Group = *pc_Group;
            c_Group.c_SubNodeIndexes.clear();
            c_Group.c_SubNodeIndexes.reserve(c_Indices.size());
            for (uint32_t u32_ItNode = 0UL; u32_ItNode < c_Indices.size(); ++u32_ItNode)
            {
               const C_OscNode * const pc_OscNode =
                  C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(c_Indices[u32_ItNode]);
               const C_PuiSdNode * const pc_UiNode =
                  C_PuiSdHandler::h_GetInstance()->GetUiNode(c_Indices[u32_ItNode]);
               if ((pc_OscNode != NULL) && (pc_UiNode != NULL))
               {
                  c_Group.c_SubNodeIndexes.push_back(orc_Snapshot.c_OscNodes.size());
                  orc_Snapshot.c_OscNodes.push_back(*pc_OscNode);
                  orc_Snapshot.c_UiNodes.push_back(*pc_UiNode);
                  C_SebBaseCopyPasteManager::mh_HandleZetValueBox(opc_NodeItemOrigin, orc_NormalizedZetValues,
                                                                  orc_Snapshot.c_UiNodes[static_cast<std::
                                                                                                     vector<C_PuiSdNode>
                                                                                                     ::
                                                                                                     size_type>(
                                                                                            orc_Snapshot.
                                                                                            c_UiNodes
                                                                                            .
                                                                                            size()
                                                                                            -
                                                                                            1UL)]);
               }
            }
            tgl_assert(c_Group.c_SubNodeIndexes.size() == pc_Group->c_SubNodeIndexes.size());
            orc_Snapshot.c_OscNodeGroups.push_back(c_Group);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove connection from snap shot

   \param[in,out]  orc_Data            Snap shot data
   \param[in]      oru32_NodeIndex     Node index
   \param[in]      orc_ConnectionId    Connection ID
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManTopologyCopyPasteManager::mh_RemoveConnection(C_SdTopologyDataSnapshot & orc_Data,
                                                          const uint32_t & oru32_NodeIndex,
                                                          const C_PuiSdNodeConnectionId & orc_ConnectionId)
{
   if (oru32_NodeIndex < orc_Data.c_OscNodes.size())
   {
      C_OscNode & rc_OscNode = orc_Data.c_OscNodes[oru32_NodeIndex];

      rc_OscNode.c_Properties.DisconnectComInterface(orc_ConnectionId.e_InterfaceType,
                                                     orc_ConnectionId.u8_InterfaceNumber);
   }
   if (oru32_NodeIndex < orc_Data.c_UiNodes.size())
   {
      C_PuiSdNode & rc_UiNode = orc_Data.c_UiNodes[oru32_NodeIndex];

      rc_UiNode.DeleteConnection(orc_ConnectionId);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Remove CANopen information

   \param[in,out]  orc_Data   Data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManTopologyCopyPasteManager::mh_RemoveCanOpenInformation(C_SdTopologyDataSnapshot & orc_Data)
{
   tgl_assert(orc_Data.c_OscNodes.size() == orc_Data.c_UiNodes.size());
   if (orc_Data.c_OscNodes.size() == orc_Data.c_UiNodes.size())
   {
      for (uint32_t u32_ItNode = 0UL; u32_ItNode < orc_Data.c_OscNodes.size(); ++u32_ItNode)
      {
         C_OscNode & rc_OscNode = orc_Data.c_OscNodes[u32_ItNode];
         C_PuiSdNode & rc_UiNode = orc_Data.c_UiNodes[u32_ItNode];

         //CANopen
         rc_OscNode.c_CanOpenManagers.clear();

         tgl_assert(rc_OscNode.c_ComProtocols.size() == rc_UiNode.c_UiCanProtocols.size());
         if (rc_OscNode.c_ComProtocols.size() == rc_UiNode.c_UiCanProtocols.size())
         {
            for (uint32_t u32_ItProt = 0UL; u32_ItProt < rc_OscNode.c_ComProtocols.size();)
            {
               C_OscCanProtocol & rc_OscProtocol = rc_OscNode.c_ComProtocols[u32_ItProt];
               if (rc_OscProtocol.e_Type == C_OscCanProtocol::eCAN_OPEN)
               {
                  //DP
                  tgl_assert(rc_OscNode.c_DataPools.size() == rc_UiNode.c_UiDataPools.size());
                  if (rc_OscNode.c_DataPools.size() == rc_UiNode.c_UiDataPools.size())
                  {
                     rc_OscNode.c_DataPools.erase(rc_OscNode.c_DataPools.begin() + rc_OscProtocol.u32_DataPoolIndex);
                     rc_UiNode.c_UiDataPools.erase(rc_UiNode.c_UiDataPools.begin() + rc_OscProtocol.u32_DataPoolIndex);
                  }
                  //Protocol
                  rc_OscNode.c_ComProtocols.erase(rc_OscNode.c_ComProtocols.begin() + u32_ItProt);
                  rc_UiNode.c_UiCanProtocols.erase(rc_UiNode.c_UiCanProtocols.begin() + u32_ItProt);
               }
               else
               {
                  ++u32_ItProt;
               }
            }
         }
      }
   }
}
