//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Add and delete base class for system definition items (implementation)

   These classes will probably require a lot of common functions

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <limits>
#include "stwtypes.h"
#include "stwerrors.h"
#include "C_SdManUnoTopologyAddDeleteBaseCommand.h"
#include "C_SebUtil.h"
#include "C_GiNode.h"
#include "C_GiLiBus.h"
#include "C_PuiSdUtil.h"
#include "C_GiLiBusConnector.h"
#include "C_GiBiArrow.h"
#include "C_GiBiBoundary.h"
#include "C_GiBiImageGroup.h"
#include "C_GiBiTextElement.h"
#include "C_GiTextElementBus.h"
#include "C_SdTopologyScene.h"
#include "C_PuiSdHandler.h"
#include "TGLUtils.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_tgl;
using namespace std;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in,out]  opc_Scene                 Pointer to currently active scene
   \param[in]      orc_IDs                   Affected unique IDs
   \param[in]      orc_Text                  Command description
   \param[in,out]  opc_Parent                Optional pointer to parent
   \param[in]      orc_InitialSnapshotData   Initial snapshot data
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdManUnoTopologyAddDeleteBaseCommand::C_SdManUnoTopologyAddDeleteBaseCommand(QGraphicsScene * const opc_Scene,
                                                                               const std::vector<stw_types::uint64> & orc_IDs, const QString & orc_Text, QUndoCommand * const opc_Parent,
                                                                               const C_SdTopologyDataSnapshot & orc_InitialSnapshotData)
   :
   QObject(),
   C_SebUnoAddDeleteBaseCommand(opc_Scene, orc_IDs, orc_Text, opc_Parent),
   mc_DataBackup(orc_InitialSnapshotData)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdManUnoTopologyAddDeleteBaseCommand::~C_SdManUnoTopologyAddDeleteBaseCommand(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Filter all relevant objects

   \param[in]   orc_AllItems           All available items
   \param[in]   orc_SelectedItems      All selected items
   \param[out]  orc_AllRelevantItems   Filtered items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyAddDeleteBaseCommand::h_GetAllRelevantObjects(const QList<QGraphicsItem *> & orc_AllItems,
                                                                     const QList<QGraphicsItem *> & orc_SelectedItems,
                                                                     QList<QGraphicsItem *> & orc_AllRelevantItems)
{
   orc_AllRelevantItems.clear();
   for (QList<QGraphicsItem *>::const_iterator c_ItSelectedItem = orc_SelectedItems.begin();
        c_ItSelectedItem != orc_SelectedItems.end(); ++c_ItSelectedItem)
   {
      QGraphicsItem * const pc_CurrentItem = C_SebUtil::h_GetHighestParent(*c_ItSelectedItem);

      C_GiUnique * const pc_UniqueElement = dynamic_cast<C_GiUnique *>(pc_CurrentItem);
      if (pc_UniqueElement != NULL)
      {
         C_GiNode * const pc_Node = dynamic_cast<C_GiNode *>(pc_CurrentItem);

         C_GiLiBus * const pc_Bus = dynamic_cast<C_GiLiBus *>(pc_CurrentItem);
         //Item is unique
         orc_AllRelevantItems.push_back(pc_CurrentItem);
         //Add all bus connectors if necessary
         if (pc_Node != NULL)
         {
            mh_SearchAndAddAllAffectedBusConnectorsToTheFront(orc_AllItems, -1, pc_Node->GetIndex(),
                                                              orc_AllRelevantItems);
         }
         if (pc_Bus != NULL)
         {
            mh_SearchAndAddAllAffectedBusConnectorsToTheFront(orc_AllItems, pc_Bus->GetIndex(), -1,
                                                              orc_AllRelevantItems);
            mh_SearchAndAddAllAffectedBusTextElementsToTheFront(orc_AllItems, static_cast<uint32>(pc_Bus->GetIndex()),
                                                                orc_AllRelevantItems);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get backup data

   \return
   Current backup data
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdTopologyDataSnapshot C_SdManUnoTopologyAddDeleteBaseCommand::GetDataBackup() const
{
   return this->mc_DataBackup;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete all relevant items and save them to internal backup
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyAddDeleteBaseCommand::m_DeleteSave(void)
{
   m_SaveToData();
   m_Delete();
   Q_EMIT (this->SigErrorChange());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Restore all items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyAddDeleteBaseCommand::m_Restore(void)
{
   C_SdTopologyScene * const pc_Scene = dynamic_cast<C_SdTopologyScene * const>(mpc_Scene);

   if (pc_Scene != NULL)
   {
      pc_Scene->CopyFromSnapshotToScene(this->mc_DataBackup, &(this->mc_MapTypeAndIndexToID));
      Q_EMIT (this->SigErrorChange());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get boundary element type

   \return
   Boundary element type
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdManUnoTopologyAddDeleteBaseCommand::m_GetBoundaryType(void) const
{
   return static_cast<sint32>(C_PuiSdDataElement::eBOUNDARY);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get image element type

   \return
   Image element type
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdManUnoTopologyAddDeleteBaseCommand::m_GetImageType(void) const
{
   return static_cast<sint32>(C_PuiSdDataElement::eIMAGE);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get line arrow element type

   \return
   Line arrow element type
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdManUnoTopologyAddDeleteBaseCommand::m_GetLineArrowType(void) const
{
   return static_cast<sint32>(C_PuiSdDataElement::eLINE_ARROW);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get text element type

   \return
   Text element type
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdManUnoTopologyAddDeleteBaseCommand::m_GetTextElementType(void) const
{
   return static_cast<sint32>(C_PuiSdDataElement::eTEXT_ELEMENT);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Apply position offset

   \param[in]  orc_NewPos  Offset
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyAddDeleteBaseCommand::m_SetDataPositionOffset(const QPointF & orc_NewPos)
{
   this->mc_DataBackup.SetDataPositionOffset(orc_NewPos);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Apply Z value offset

   \param[in]  of64_HighestUsedZValue  Highest used Z value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyAddDeleteBaseCommand::m_SetDataZOffset(const float64 of64_HighestUsedZValue)
{
   this->mc_DataBackup.SetDataZOffset(of64_HighestUsedZValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add all busses which are under influence of the selected node or bus

   \param[in]   orc_AllItems           All available items
   \param[in]   ors32_BusIndex         Bus index
   \param[in]   ors32_NodeIndex        Node index
   \param[out]  orc_AllRelevantItems   Filtered items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyAddDeleteBaseCommand::mh_SearchAndAddAllAffectedBusConnectorsToTheFront(
   const QList<QGraphicsItem *> & orc_AllItems, const sint32 & ors32_BusIndex, const sint32 & ors32_NodeIndex,
   QList<QGraphicsItem *> & orc_AllRelevantItems)
{
   for (QList<QGraphicsItem *>::const_iterator c_ItItem = orc_AllItems.begin();
        c_ItItem != orc_AllItems.end(); ++c_ItItem)
   {
      //Bus connector
      QGraphicsItem * const pc_CurrentItem = C_SebUtil::h_GetHighestParent(*c_ItItem);

      C_GiLiBusConnector * const pc_BusConnector = dynamic_cast<C_GiLiBusConnector *>(pc_CurrentItem);
      if (pc_BusConnector != NULL)
      {
         C_GiNode * const pc_Node = pc_BusConnector->GetNodeItem();
         const C_GiLiBus * const pc_Bus = pc_BusConnector->GetBusItem();
         //Node match
         if (pc_Node != NULL)
         {
            if (ors32_NodeIndex == pc_Node->GetIndex())
            {
               orc_AllRelevantItems.push_front(pc_CurrentItem);
            }
         }
         //Bus match
         if (pc_Bus != NULL)
         {
            if (ors32_BusIndex == pc_Bus->GetIndex())
            {
               orc_AllRelevantItems.push_front(pc_CurrentItem);
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add all busses which are under influence of the selected node or bus

   \param[in]   orc_AllItems           All available items
   \param[in]   ou32_BusIndex          Unique bus index
   \param[out]  orc_AllRelevantItems   Filtered items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyAddDeleteBaseCommand::mh_SearchAndAddAllAffectedBusTextElementsToTheFront(
   const QList<QGraphicsItem *> & orc_AllItems, const uint32 ou32_BusIndex,
   QList<QGraphicsItem *> & orc_AllRelevantItems)
{
   QList<QGraphicsItem *>::const_iterator c_ItItem;

   for (c_ItItem = orc_AllItems.begin(); c_ItItem != orc_AllItems.end(); ++c_ItItem)
   {
      //Bus connector
      QGraphicsItem * const pc_CurrentItem = C_SebUtil::h_GetHighestParent(*c_ItItem);

      C_GiTextElementBus * const pc_TextElementBus = dynamic_cast<C_GiTextElementBus *>(pc_CurrentItem);
      if (pc_TextElementBus != NULL)
      {
         if (pc_TextElementBus->GetBusIndex() == ou32_BusIndex)
         {
            orc_AllRelevantItems.push_front(pc_CurrentItem);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save currently relevant system definition part to data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyAddDeleteBaseCommand::m_SaveToData(void)
{
   const vector<QGraphicsItem *> c_RelatedItems = this->m_GetSceneItems();
   C_GiNode * pc_Node;
   C_GiLiBus * pc_Bus;
   C_GiLiBusConnector * pc_BusConnector;
   C_GiTextElementBus * pc_TextElementBus;
   C_PuiSdDataElement * pc_Data;
   C_GiUnique * pc_Unique;
   uint32 u32_NodeCounter = 0UL;

   QMap<uint32, uint32> c_MapOldIndexToNewIndex;

   this->m_Clear();

   //Base elements
   m_StoreCommon(this->mc_DataBackup, this->mc_MapTypeAndIndexToID, C_PuiSdHandler::h_GetInstance()->c_Elements);
   //Other elements
   for (vector<QGraphicsItem *>::const_iterator c_ItRelatedItem = c_RelatedItems.begin();
        c_ItRelatedItem != c_RelatedItems.end(); ++c_ItRelatedItem)
   {
      pc_Unique = dynamic_cast<C_GiUnique *>(*c_ItRelatedItem);
      if (pc_Unique != NULL)
      {
         const uint64 u64_CurUniqueID = pc_Unique->GetID();
         //lint -e{740}  no problem because of common base class

         pc_Data = dynamic_cast<C_PuiSdDataElement *>(*c_ItRelatedItem);
         if (pc_Data != NULL)
         {
            const sint32 s32_Index = pc_Data->GetIndex();
            if (s32_Index >= 0)
            {
               const uint32 u32_Index = static_cast<uint32>(s32_Index);
               //Save to scene data
               pc_Data->UpdateData();

               //Backup scene data internally
               //Node
               this->m_SaveToDataNode(dynamic_cast<C_GiNode *>(*c_ItRelatedItem), u32_Index, u64_CurUniqueID,
                                      u32_NodeCounter);
               //Bus

               pc_Bus = dynamic_cast<C_GiLiBus *>(*c_ItRelatedItem);
               if (pc_Bus != NULL)
               {
                  const C_OSCSystemBus * const pc_OSCBus = C_PuiSdHandler::h_GetInstance()->GetOSCBus(u32_Index);
                  const C_PuiSdBus * const pc_UIBus = C_PuiSdHandler::h_GetInstance()->GetUIBus(u32_Index);
                  c_MapOldIndexToNewIndex.insert(u32_Index, this->mc_DataBackup.c_OSCBuses.size());
                  if (pc_OSCBus != NULL)
                  {
                     this->mc_DataBackup.c_OSCBuses.push_back(*pc_OSCBus);
                  }
                  if (pc_UIBus != NULL)
                  {
                     this->mc_DataBackup.c_UIBuses.push_back(*pc_UIBus);
                  }
                  if (this->mc_DataBackup.c_OSCBuses.size() == this->mc_DataBackup.c_UIBuses.size())
                  {
                     this->mc_MapTypeAndIndexToID.insert(C_PuiBsTemporaryDataID(static_cast<sint32>(C_PuiSdDataElement::
                                                                                                    E_Type::eBUS),
                                                                                this->mc_DataBackup.c_UIBuses.size()
                                                                                -
                                                                                1UL),
                                                         u64_CurUniqueID);
                  }
               }

               // Bus text element

               pc_TextElementBus = dynamic_cast<C_GiTextElementBus *>(*c_ItRelatedItem);
               if (pc_TextElementBus != NULL)
               {
                  if (u32_Index < C_PuiSdHandler::h_GetInstance()->c_BusTextElements.size())
                  {
                     this->mc_DataBackup.c_BusTextElements.push_back(C_PuiSdHandler::h_GetInstance()->c_BusTextElements[
                                                                        u32_Index]);
                     this->mc_MapTypeAndIndexToID.insert(C_PuiBsTemporaryDataID(static_cast<sint32>(C_PuiSdDataElement::
                                                                                                    E_Type::
                                                                                                    eTEXT_ELEMENT_BUS),
                                                                                this->mc_DataBackup.c_BusTextElements.
                                                                                size() - 1UL),
                                                         u64_CurUniqueID);
                  }
               }
            }
         }
         else
         {
            //Bus connector

            pc_BusConnector = dynamic_cast<C_GiLiBusConnector *>(*c_ItRelatedItem);
            if (pc_BusConnector != NULL)
            {
               const C_GiLiBus * pc_BusConst;
               const uint32 u32_BackupBusConnectionIndex = this->mc_DataBackup.c_BusConnections.size();
               this->mc_DataBackup.c_BusConnections.push_back(C_PuiSdCompleteBusConnectionData());
               {
                  C_PuiSdCompleteBusConnectionData & rc_CurBusConnectionBackupData =
                     this->mc_DataBackup.c_BusConnections[u32_BackupBusConnectionIndex];
                  this->mc_MapTypeAndIndexToID.insert(C_PuiBsTemporaryDataID(static_cast<sint32>(
                                                                                C_PuiSdDataElement::
                                                                                E_Type::
                                                                                eBUS_CONNECTOR),
                                                                             u32_BackupBusConnectionIndex),
                                                      u64_CurUniqueID);
                  //Node data
                  pc_Node = pc_BusConnector->GetNodeItem();
                  if (pc_Node != NULL)
                  {
                     const sint32 s32_NodeIndex = pc_Node->GetIndex();

                     //Trigger update node data again to be save in the case only the bus of a bus connector was
                     // selected
                     pc_Node->UpdateData();
                     rc_CurBusConnectionBackupData.u64_NodeID = pc_Node->GetID();
                     if (s32_NodeIndex >= 0)
                     {
                        const uint32 u32_NodeIndex = static_cast<uint32>(s32_NodeIndex);
                        const sint32 s32_BusConnectionDataIndex = pc_Node->GetIndexOfConnector(pc_BusConnector);
                        if (s32_BusConnectionDataIndex >= 0)
                        {
                           const uint32 u32_BusConnectionDataIndex = static_cast<uint32>(s32_BusConnectionDataIndex);
                           const C_PuiSdNode * const pc_UINode =
                              C_PuiSdHandler::h_GetInstance()->GetUINode(u32_NodeIndex);
                           //Core
                           const C_PuiSdNodeConnectionId * const pc_CurConnId = pc_Node->GetNodeConnectionId(
                              pc_BusConnector);
                           if (pc_CurConnId != NULL)
                           {
                              rc_CurBusConnectionBackupData.c_ConnectionId = *pc_CurConnId;
                           }
                           //UI
                           if (pc_UINode != NULL)
                           {
                              if (u32_BusConnectionDataIndex < pc_UINode->c_UIBusConnections.size())
                              {
                                 rc_CurBusConnectionBackupData.c_UIData =
                                    pc_UINode->c_UIBusConnections[u32_BusConnectionDataIndex];
                              }
                           }
                        }
                     }
                  }
                  //Bus data
                  pc_BusConst = pc_BusConnector->GetBusItem();
                  if (pc_BusConst != NULL)
                  {
                     rc_CurBusConnectionBackupData.u64_BusID = pc_BusConst->GetID();
                  }
               }
            }
         }
      }
   }
   //Clear node connectors to avoid duplicate creation
   for (uint32 u32_It = 0; u32_It < this->mc_DataBackup.c_UINodes.size(); ++u32_It)
   {
      C_OSCNode & rc_OSCNode = this->mc_DataBackup.c_OSCNodes[u32_It];
      C_PuiSdNode & rc_UINode = this->mc_DataBackup.c_UINodes[u32_It];
      for (uint32 u32_ItComInterface = 0; u32_ItComInterface < rc_OSCNode.c_Properties.c_ComInterfaces.size();
           ++u32_ItComInterface)
      {
         C_OSCNodeComInterfaceSettings & rc_CurInterface =
            rc_OSCNode.c_Properties.c_ComInterfaces[u32_ItComInterface];
         rc_CurInterface.RemoveConnection();
      }
      rc_UINode.c_UIBusConnections.clear();
   }
   //Adapt text element ids
   for (uint32 u32_ItBusTextElement = 0; u32_ItBusTextElement < this->mc_DataBackup.c_BusTextElements.size();
        ++u32_ItBusTextElement)
   {
      C_PuiSdTextElementBus & rc_BusTextElement = this->mc_DataBackup.c_BusTextElements[u32_ItBusTextElement];
      rc_BusTextElement.u32_BusIndex = c_MapOldIndexToNewIndex.value(rc_BusTextElement.u32_BusIndex,
                                                                     std::numeric_limits<uint32>::max());
      tgl_assert(rc_BusTextElement.u32_BusIndex != std::numeric_limits<uint32>::max());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save to data, node

   \param[in]   opc_UiNodeItem      UI node item
   \param[in]   ou32_NodeIndex      Node index
   \param[in]   ou64_UniqueID       Unique ID
   \param[out]  oru32_NodeCounter   Node counter
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyAddDeleteBaseCommand::m_SaveToDataNode(const C_GiNode * const opc_UiNodeItem,
                                                              const uint32 ou32_NodeIndex, const uint64 ou64_UniqueID,
                                                              uint32 & oru32_NodeCounter)
{
   if (opc_UiNodeItem != NULL)
   {
      const bool q_IsMulti = C_OSCNodeSquad::h_CheckIsMultiDevice(ou32_NodeIndex,
                                                                  C_PuiSdHandler::h_GetInstance()->GetOSCSystemDefinitionConst().c_NodeSquads);
      if (q_IsMulti)
      {
         this->m_SaveToDataMultiNode(ou32_NodeIndex);
      }
      else
      {
         const C_OSCNode * const pc_OSCNode =
            C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(ou32_NodeIndex);
         const C_PuiSdNode * const pc_UINode =
            C_PuiSdHandler::h_GetInstance()->GetUINode(ou32_NodeIndex);
         if (pc_OSCNode != NULL)
         {
            this->mc_DataBackup.c_OSCNodes.push_back(*pc_OSCNode);
         }
         if (pc_UINode != NULL)
         {
            this->mc_DataBackup.c_UINodes.push_back(*pc_UINode);
         }
      }
      if (this->mc_DataBackup.c_OSCNodes.size() == this->mc_DataBackup.c_UINodes.size())
      {
         this->mc_MapTypeAndIndexToID.insert(C_PuiBsTemporaryDataID(static_cast<sint32>(
                                                                       C_PuiSdDataElement::
                                                                       E_Type::eNODE), oru32_NodeCounter),
                                             ou64_UniqueID);
      }
      ++oru32_NodeCounter;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save to data, multi node

   \param[in]  ou32_NodeIndex    Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyAddDeleteBaseCommand::m_SaveToDataMultiNode(const uint32 ou32_NodeIndex)
{
   uint32 u32_GroupIndex;

   if (C_PuiSdHandler::h_GetInstance()->GetNodeSquadIndexWithNodeIndex(ou32_NodeIndex,
                                                                       u32_GroupIndex) == C_NO_ERR)
   {
      const C_OSCNodeSquad * const pc_Group = C_PuiSdHandler::h_GetInstance()->GetOSCNodeSquadConst(
         u32_GroupIndex);
      if (pc_Group != NULL)
      {
         const std::vector<uint32> c_Indices = C_PuiSdHandler::h_GetInstance()->GetAllNodeGroupIndicesUsingNodeIndex(
            ou32_NodeIndex);
         C_OSCNodeSquad c_Group = *pc_Group;
         c_Group.c_SubNodeIndexes.clear();
         c_Group.c_SubNodeIndexes.reserve(c_Indices.size());
         for (uint32 u32_ItNode = 0UL; u32_ItNode < c_Indices.size(); ++u32_ItNode)
         {
            const C_OSCNode * const pc_OSCNode =
               C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(c_Indices[u32_ItNode]);
            const C_PuiSdNode * const pc_UINode =
               C_PuiSdHandler::h_GetInstance()->GetUINode(c_Indices[u32_ItNode]);
            c_Group.c_SubNodeIndexes.push_back(this->mc_DataBackup.c_OSCNodes.size());
            if (pc_OSCNode != NULL)
            {
               this->mc_DataBackup.c_OSCNodes.push_back(*pc_OSCNode);
            }
            if (pc_UINode != NULL)
            {
               this->mc_DataBackup.c_UINodes.push_back(*pc_UINode);
            }
         }
         this->mc_DataBackup.c_OSCNodeGroups.push_back(c_Group);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clear all data storage
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyAddDeleteBaseCommand::m_Clear(void)
{
   this->mc_MapTypeAndIndexToID.clear();
   this->mc_DataBackup.Clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete all relevant items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyAddDeleteBaseCommand::m_Delete(void)
{
   C_SdTopologyScene * const pc_Scene = dynamic_cast<C_SdTopologyScene * const>(mpc_Scene);

   if (pc_Scene != NULL)
   {
      const vector<QGraphicsItem *> c_Items = this->m_GetSceneItems();
      for (vector<QGraphicsItem *>::const_iterator c_ItItem = c_Items.begin(); c_ItItem != c_Items.end(); ++c_ItItem)
      {
         pc_Scene->DeleteItem(*c_ItItem);
      }
   }
}
