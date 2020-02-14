//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handle most parts of copy paste implementation for scene (implementation)

   Handle most parts of copy paste implementation for scene

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <limits>
#include "stwerrors.h"
#include "C_SdManTopologyCopyPasteManager.h"

#include "C_GiNode.h"
#include "C_GiLiBus.h"
#include "C_GiSdImageGroup.h"
#include "C_GiSdBoundary.h"
#include "C_GiSdArrow.h"
#include "C_GiSdTextElement.h"
#include "C_GiTextElementBus.h"
#include "C_SdClipBoardHelper.h"
#include "C_PuiSdHandler.h"
#include "C_SdTopologyDataSnapshot.h"

#include "gitypes.h"
#include "C_SebUtil.h"
#include "TGLUtils.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_tgl;

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

   \param[in] orc_SelectedItems     Selected items to copy
   \param[in] orc_NormalizedZValues Normalized Z values for all copied items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManTopologyCopyPasteManager::CopyFromSceneToManager(const QList<QGraphicsItem *> & orc_SelectedItems,
                                                             const QMap<const QGraphicsItem *,
                                                                        float64> & orc_NormalizedZValues)
{
   C_SdTopologyDataSnapshot c_Snapshot;
   uint32 u32_Index;
   const C_GiNode * pc_Node;
   const C_GiLiBus * pc_Bus;
   const C_GiSdImageGroup * pc_Image;
   const C_GiSdBoundary * pc_Boundary;
   const C_GiSdArrow * pc_LineArrow;
   const C_GiSdTextElement * pc_TextElement;
   const C_GiTextElementBus * pc_TextElementBus;

   QMap<uint32, uint32> c_MapOldIndexToNewIndex;

   std::vector<const QGraphicsItem *> c_HandledItems;

   //Handle Z order

   //Clear data
   for (QList<QGraphicsItem *>::const_iterator c_ItItem = orc_SelectedItems.begin();
        c_ItItem != orc_SelectedItems.end();
        ++c_ItItem)
   {
      bool q_Handled = false;
      const QGraphicsItem * const pc_CurItem = C_SebUtil::h_GetHighestParent(*c_ItItem);
      for (uint32 u32_ItHandledItems = 0; u32_ItHandledItems < c_HandledItems.size(); ++u32_ItHandledItems)
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
         case msn_GRAPHICS_ITEM_NODE:
            //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
            pc_Node = dynamic_cast<const C_GiNode *>(pc_CurItem);
            if (pc_Node != NULL)
            {
               u32_Index = static_cast<uint32>(pc_Node->GetIndex());
               const C_OSCNode * const pc_OSCNode = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(u32_Index);
               const C_PuiSdNode * const pc_UINode = C_PuiSdHandler::h_GetInstance()->GetUINode(u32_Index);
               if ((pc_OSCNode != NULL) && (pc_UINode != NULL))
               {
                  c_Snapshot.c_OSCNodes.push_back(*pc_OSCNode);
                  c_Snapshot.c_UINodes.push_back(*pc_UINode);
                  C_SebBaseCopyPasteManager::mh_HandleZValueBox(*c_ItItem, orc_NormalizedZValues,
                                                                c_Snapshot.c_UINodes[static_cast<std::vector<C_PuiSdNode>
                                                                                                 ::size_type>(
                                                                                        c_Snapshot.c_UINodes.size() -
                                                                                        1UL)]);
               }
            }
            break;
         case msn_GRAPHICS_ITEM_CANBUS: //Same data element
         case msn_GRAPHICS_ITEM_ETHERNETBUS:
            //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
            pc_Bus = dynamic_cast<const C_GiLiBus *>(pc_CurItem);
            if (pc_Bus != NULL)
            {
               u32_Index = static_cast<uint32>(pc_Bus->GetIndex());
               const C_OSCSystemBus * const pc_OSCBus = C_PuiSdHandler::h_GetInstance()->GetOSCBus(u32_Index);
               const C_PuiSdBus * const pc_UIBus = C_PuiSdHandler::h_GetInstance()->GetUIBus(u32_Index);
               if ((pc_OSCBus != NULL) && (pc_UIBus != NULL))
               {
                  c_MapOldIndexToNewIndex.insert(u32_Index, c_Snapshot.c_OSCBuses.size());
                  c_Snapshot.c_OSCBuses.push_back(*pc_OSCBus);
                  c_Snapshot.c_UIBuses.push_back(*pc_UIBus);
                  C_SebBaseCopyPasteManager::mh_HandleZValueLine(*c_ItItem, orc_NormalizedZValues,
                                                                 c_Snapshot.c_UIBuses[static_cast<std::vector<C_PuiSdBus>
                                                                                                  ::size_type>(
                                                                                         c_Snapshot.
                                                                                         c_UIBuses
                                                                                         .size() -
                                                                                         1UL)]);
               }
            }
            break;
         case msn_GRAPHICS_ITEM_TEXTELEMENT:
            //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
            pc_TextElement = dynamic_cast<const C_GiSdTextElement *>(pc_CurItem);
            if (pc_TextElement != NULL)
            {
               u32_Index = static_cast<uint32>(pc_TextElement->GetIndex());
               if (u32_Index < C_PuiSdHandler::h_GetInstance()->c_Elements.c_TextElements.size())
               {
                  c_Snapshot.c_TextElements.push_back(C_PuiSdHandler::h_GetInstance()->c_Elements.c_TextElements[
                                                         u32_Index]);
                  C_SebBaseCopyPasteManager::mh_HandleZValueBox(*c_ItItem, orc_NormalizedZValues,
                                                                c_Snapshot.c_TextElements[static_cast<std::vector<C_PuiBsTextElement>
                                                                                                      ::size_type>(
                                                                                             c_Snapshot.c_TextElements.
                                                                                             size()
                                                                                             -
                                                                                             1UL)]);
               }
            }
            break;
         case msn_GRAPHICS_ITEM_TEXTELEMENT_BUS:
            //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
            pc_TextElementBus = dynamic_cast<const C_GiTextElementBus *>(pc_CurItem);
            if (pc_TextElementBus != NULL)
            {
               u32_Index = static_cast<uint32>(pc_TextElementBus->GetIndex());
               if (u32_Index < C_PuiSdHandler::h_GetInstance()->c_BusTextElements.size())
               {
                  c_Snapshot.c_BusTextElements.push_back(C_PuiSdHandler::h_GetInstance()->c_BusTextElements[u32_Index]);
                  C_SebBaseCopyPasteManager::mh_HandleZValueBox(*c_ItItem, orc_NormalizedZValues,
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
         case msn_GRAPHICS_ITEM_BOUNDARY:
            //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
            pc_Boundary = dynamic_cast<const C_GiSdBoundary *>(pc_CurItem);
            if (pc_Boundary != NULL)
            {
               u32_Index = static_cast<uint32>(pc_Boundary->GetIndex());
               if (u32_Index < C_PuiSdHandler::h_GetInstance()->c_Elements.c_Boundaries.size())
               {
                  c_Snapshot.c_Boundaries.push_back(C_PuiSdHandler::h_GetInstance()->c_Elements.c_Boundaries[u32_Index]);
                  C_SebBaseCopyPasteManager::mh_HandleZValueBox(*c_ItItem, orc_NormalizedZValues,
                                                                c_Snapshot.c_Boundaries[static_cast<std::vector<C_PuiBsBoundary>
                                                                                                    ::size_type>(
                                                                                           c_Snapshot.
                                                                                           c_Boundaries
                                                                                           .size() -
                                                                                           1UL)]);
               }
            }
            break;
         case msn_GRAPHICS_ITEM_IMAGE:
            //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
            pc_Image = dynamic_cast<const C_GiSdImageGroup *>(pc_CurItem);
            if (pc_Image != NULL)
            {
               u32_Index = static_cast<uint32>(pc_Image->GetIndex());
               if (u32_Index < C_PuiSdHandler::h_GetInstance()->c_Elements.c_Images.size())
               {
                  c_Snapshot.c_Images.push_back(C_PuiSdHandler::h_GetInstance()->c_Elements.c_Images[u32_Index]);
                  C_SebBaseCopyPasteManager::mh_HandleZValueBox(*c_ItItem, orc_NormalizedZValues,
                                                                c_Snapshot.c_Images[static_cast<std::vector<C_PuiBsImage>
                                                                                                ::size_type>(c_Snapshot.
                                                                                                             c_Images.
                                                                                                             size() -
                                                                                                             1UL)]);
               }
            }
            break;
         case msn_GRAPHICS_ITEM_LINE_ARROW:
            //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
            pc_LineArrow = dynamic_cast<const C_GiSdArrow *>(pc_CurItem);
            if (pc_LineArrow != NULL)
            {
               u32_Index = static_cast<uint32>(pc_LineArrow->GetIndex());
               if (u32_Index < C_PuiSdHandler::h_GetInstance()->c_Elements.c_LineArrows.size())
               {
                  c_Snapshot.c_LineArrows.push_back(C_PuiSdHandler::h_GetInstance()->c_Elements.c_LineArrows[u32_Index]);
                  C_SebBaseCopyPasteManager::mh_HandleZValueLine(*c_ItItem, orc_NormalizedZValues,
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
   for (uint32 u32_ItNode = 0; u32_ItNode < c_Snapshot.c_OSCNodes.size(); ++u32_ItNode)
   {
      C_OSCNode & rc_Node = c_Snapshot.c_OSCNodes[u32_ItNode];
      for (uint32 u32_ItInterface = 0; u32_ItInterface < rc_Node.c_Properties.c_ComInterfaces.size(); ++u32_ItInterface)
      {
         C_OSCNodeComInterfaceSettings & rc_ComInterface = rc_Node.c_Properties.c_ComInterfaces[u32_ItInterface];
         if (rc_ComInterface.q_IsBusConnected == true)
         {
            //Check if bus was copied
            //Replace
            rc_ComInterface.u32_BusIndex = c_MapOldIndexToNewIndex.value(rc_ComInterface.u32_BusIndex,
                                                                         std::numeric_limits<uint32>::max());
            if (rc_ComInterface.u32_BusIndex == std::numeric_limits<uint32>::max())
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
   for (uint32 u32_ItBusTextElement = 0; u32_ItBusTextElement < c_Snapshot.c_BusTextElements.size();
        ++u32_ItBusTextElement)
   {
      C_PuiSdTextElementBus & rc_BusTextElement = c_Snapshot.c_BusTextElements[u32_ItBusTextElement];
      //Replace
      rc_BusTextElement.u32_BusIndex = c_MapOldIndexToNewIndex.value(rc_BusTextElement.u32_BusIndex,
                                                                     std::numeric_limits<uint32>::max());
      tgl_assert(rc_BusTextElement.u32_BusIndex != std::numeric_limits<uint32>::max());
      if (rc_BusTextElement.u32_BusIndex == std::numeric_limits<uint32>::max())
      {
         c_Snapshot.c_BusTextElements.erase(c_Snapshot.c_BusTextElements.begin() + u32_ItBusTextElement);
         //No iteration step as new element is at current position
         --u32_ItBusTextElement;
      }
   }
   //Handle position of copied items
   this->m_CalcOriginalPosition(&c_Snapshot);
   //reset paste counter
   this->mu32_PasteCounter = 0;

   //Copy to clip board
   C_SdClipBoardHelper::h_StoreDataSnapShotToClipboard(c_Snapshot);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Indicator if copy paste manager has some content

   Warning: is expected to be called bevore getting any data

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

   \param[in] opc_Data Data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManTopologyCopyPasteManager::m_CalcOriginalPosition(const C_PuiBsElements * const opc_Data)
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   const C_SdTopologyDataSnapshot * const pc_ExpectedData =
      dynamic_cast<const C_SdTopologyDataSnapshot * const>(opc_Data);

   C_SebBaseCopyPasteManager::m_CalcOriginalPosition(opc_Data);
   if (pc_ExpectedData != NULL)
   {
      uint32 u32_ItElem;

      for (u32_ItElem = 0; u32_ItElem < pc_ExpectedData->c_UIBuses.size(); ++u32_ItElem)
      {
         const C_PuiSdBus & rc_Bus = pc_ExpectedData->c_UIBuses[u32_ItElem];
         for (uint32 u32_ItConnPoint = 0; u32_ItConnPoint < rc_Bus.c_UIInteractionPoints.size(); ++u32_ItConnPoint)
         {
            m_MinToOrgPos(rc_Bus.c_UIInteractionPoints[u32_ItConnPoint]);
         }
      }

      for (u32_ItElem = 0; u32_ItElem < pc_ExpectedData->c_UINodes.size(); ++u32_ItElem)
      {
         const C_PuiSdNode & rc_Node = pc_ExpectedData->c_UINodes[u32_ItElem];
         m_MinToOrgPos(rc_Node.c_UIPosition);

         for (uint32 u32_ItConn = 0; u32_ItConn < rc_Node.c_UIBusConnections.size(); ++u32_ItConn)
         {
            for (uint32 u32_ItConnPoint = 0;
                 u32_ItConnPoint < rc_Node.c_UIBusConnections[u32_ItConn].c_UINodeConnectionInteractionPoints.size();
                 ++u32_ItConnPoint)
            {
               mh_Min(this->mc_OriginalPosition,
                      rc_Node.c_UIBusConnections[u32_ItConn].c_UINodeConnectionInteractionPoints[u32_ItConnPoint]);
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove connection from snap shot

   \param[in,out] orc_Data         Snap shot data
   \param[in]     oru32_NodeIndex  Node index
   \param[in]     orc_ConnectionId Connection ID
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManTopologyCopyPasteManager::mh_RemoveConnection(C_SdTopologyDataSnapshot & orc_Data,
                                                          const uint32 & oru32_NodeIndex,
                                                          const C_PuiSdNodeConnectionId & orc_ConnectionId)
{
   if (oru32_NodeIndex < orc_Data.c_OSCNodes.size())
   {
      C_OSCNode & rc_OSCNode = orc_Data.c_OSCNodes[oru32_NodeIndex];

      rc_OSCNode.c_Properties.DisconnectComInterface(orc_ConnectionId.e_InterfaceType,
                                                     orc_ConnectionId.u8_InterfaceNumber);
   }
   if (oru32_NodeIndex < orc_Data.c_UINodes.size())
   {
      C_PuiSdNode & rc_UINode = orc_Data.c_UINodes[oru32_NodeIndex];

      rc_UINode.DeleteConnection(orc_ConnectionId);
   }
}
