//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Add and delete base class for system definition items (implementation)

   These classes will probably require a lot of common functions

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <limits>
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_SdManUnoTopologyAddDeleteBaseCommand.hpp"
#include "C_SebUtil.hpp"
#include "C_GiNode.hpp"
#include "C_GiLiBus.hpp"
#include "C_PuiSdUtil.hpp"
#include "C_GiLiBusConnector.hpp"
#include "C_GiBiArrow.hpp"
#include "C_GiBiBoundary.hpp"
#include "C_GiBiImageGroup.hpp"
#include "C_GiBiTextElement.hpp"
#include "C_GiTextElementBus.hpp"
#include "C_SdTopologyScene.hpp"
#include "C_PuiSdHandler.hpp"
#include "TglUtils.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::tgl;
using namespace std;
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

   \param[in,out]  opc_Scene                 Pointer to currently active scene
   \param[in]      orc_Ids                   Affected unique IDs
   \param[in]      orc_Text                  Command description
   \param[in,out]  opc_Parent                Optional pointer to parent
   \param[in]      orc_InitialSnapshotData   Initial snapshot data
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdManUnoTopologyAddDeleteBaseCommand::C_SdManUnoTopologyAddDeleteBaseCommand(QGraphicsScene * const opc_Scene,
                                                                               const std::vector<uint64_t> & orc_Ids,
                                                                               const QString & orc_Text,
                                                                               QUndoCommand * const opc_Parent,
                                                                               const C_SdTopologyDataSnapshot & orc_InitialSnapshotData)
   :
   QObject(),
   C_SebUnoAddDeleteBaseCommand(opc_Scene, orc_Ids, orc_Text, opc_Parent),
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
            mh_SearchAndAddAllAffectedBusTextElementsToTheFront(orc_AllItems, static_cast<uint32_t>(pc_Bus->GetIndex()),
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
   m_HandleCanOpenBeforeDelete();
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
      pc_Scene->CopyFromSnapshotToScene(this->mc_DataBackup, &(this->mc_MapTypeAndIndexToId));
      Q_EMIT (this->SigErrorChange());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get boundary element type

   \return
   Boundary element type
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdManUnoTopologyAddDeleteBaseCommand::m_GetBoundaryType(void) const
{
   return static_cast<int32_t>(C_PuiSdDataElement::eBOUNDARY);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get image element type

   \return
   Image element type
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdManUnoTopologyAddDeleteBaseCommand::m_GetImageType(void) const
{
   return static_cast<int32_t>(C_PuiSdDataElement::eIMAGE);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get line arrow element type

   \return
   Line arrow element type
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdManUnoTopologyAddDeleteBaseCommand::m_GetLineArrowType(void) const
{
   return static_cast<int32_t>(C_PuiSdDataElement::eLINE_ARROW);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get text element type

   \return
   Text element type
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdManUnoTopologyAddDeleteBaseCommand::m_GetTextElementType(void) const
{
   return static_cast<int32_t>(C_PuiSdDataElement::eTEXT_ELEMENT);
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

   \param[in]  of64_HighestUsedZetValue  Highest used Z value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyAddDeleteBaseCommand::m_SetDataZetOffset(const float64_t of64_HighestUsedZetValue)
{
   this->mc_DataBackup.SetDataZetOffset(of64_HighestUsedZetValue);
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
   const QList<QGraphicsItem *> & orc_AllItems, const int32_t & ors32_BusIndex, const int32_t & ors32_NodeIndex,
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
   const QList<QGraphicsItem *> & orc_AllItems, const uint32_t ou32_BusIndex,
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
   uint32_t u32_NodeCounter = 0UL;

   QMap<uint32_t, uint32_t> c_MapOldIndexToNewIndex;

   this->m_Clear();

   //Base elements
   m_StoreCommon(this->mc_DataBackup, this->mc_MapTypeAndIndexToId, C_PuiSdHandler::h_GetInstance()->c_Elements);
   //Other elements
   for (vector<QGraphicsItem *>::const_iterator c_ItRelatedItem = c_RelatedItems.begin();
        c_ItRelatedItem != c_RelatedItems.end(); ++c_ItRelatedItem)
   {
      pc_Unique = dynamic_cast<C_GiUnique *>(*c_ItRelatedItem);
      if (pc_Unique != NULL)
      {
         const uint64_t u64_CurUniqueId = pc_Unique->GetId();
         //lint -e{740}  no problem because of common base class

         pc_Data = dynamic_cast<C_PuiSdDataElement *>(*c_ItRelatedItem);
         if (pc_Data != NULL)
         {
            const int32_t s32_Index = pc_Data->GetIndex();
            if (s32_Index >= 0)
            {
               const uint32_t u32_Index = static_cast<uint32_t>(s32_Index);
               //Save to scene data
               pc_Data->UpdateData();

               //Backup scene data internally
               //Node
               this->m_SaveToDataNode(dynamic_cast<C_GiNode *>(*c_ItRelatedItem), u32_Index, u64_CurUniqueId,
                                      u32_NodeCounter);
               //Bus

               pc_Bus = dynamic_cast<C_GiLiBus *>(*c_ItRelatedItem);
               if (pc_Bus != NULL)
               {
                  const C_OscSystemBus * const pc_OscBus = C_PuiSdHandler::h_GetInstance()->GetOscBus(u32_Index);
                  const C_PuiSdBus * const pc_UiBus = C_PuiSdHandler::h_GetInstance()->GetUiBus(u32_Index);
                  c_MapOldIndexToNewIndex.insert(u32_Index, this->mc_DataBackup.c_OscBuses.size());
                  if (pc_OscBus != NULL)
                  {
                     this->mc_DataBackup.c_OscBuses.push_back(*pc_OscBus);
                  }
                  if (pc_UiBus != NULL)
                  {
                     this->mc_DataBackup.c_UiBuses.push_back(*pc_UiBus);
                  }
                  if (this->mc_DataBackup.c_OscBuses.size() == this->mc_DataBackup.c_UiBuses.size())
                  {
                     this->mc_MapTypeAndIndexToId.insert(C_PuiBsTemporaryDataId(static_cast<int32_t>(C_PuiSdDataElement
                                                                                                     ::
                                                                                                     E_Type::eBUS),
                                                                                this->mc_DataBackup.c_UiBuses.size()
                                                                                -
                                                                                1UL),
                                                         u64_CurUniqueId);
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
                     this->mc_MapTypeAndIndexToId.insert(C_PuiBsTemporaryDataId(static_cast<int32_t>(C_PuiSdDataElement
                                                                                                     ::
                                                                                                     E_Type::
                                                                                                     eTEXT_ELEMENT_BUS),
                                                                                this->mc_DataBackup.c_BusTextElements.
                                                                                size() - 1UL),
                                                         u64_CurUniqueId);
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
               const uint32_t u32_BackupBusConnectionIndex = this->mc_DataBackup.c_BusConnections.size();
               this->mc_DataBackup.c_BusConnections.push_back(C_PuiSdCompleteBusConnectionData());
               {
                  C_PuiSdCompleteBusConnectionData & rc_CurBusConnectionBackupData =
                     this->mc_DataBackup.c_BusConnections[u32_BackupBusConnectionIndex];
                  this->mc_MapTypeAndIndexToId.insert(C_PuiBsTemporaryDataId(static_cast<int32_t>(
                                                                                C_PuiSdDataElement::
                                                                                E_Type::
                                                                                eBUS_CONNECTOR),
                                                                             u32_BackupBusConnectionIndex),
                                                      u64_CurUniqueId);
                  //Node data
                  pc_Node = pc_BusConnector->GetNodeItem();
                  if (pc_Node != NULL)
                  {
                     const int32_t s32_NodeIndex = pc_Node->GetIndex();

                     //Trigger update node data again to be save in the case only the bus of a bus connector was
                     // selected
                     pc_Node->UpdateData();
                     rc_CurBusConnectionBackupData.u64_NodeId = pc_Node->GetId();
                     if (s32_NodeIndex >= 0)
                     {
                        const uint32_t u32_NodeIndex = static_cast<uint32_t>(s32_NodeIndex);
                        const int32_t s32_BusConnectionDataIndex = pc_Node->GetIndexOfConnector(pc_BusConnector);
                        if (s32_BusConnectionDataIndex >= 0)
                        {
                           const uint32_t u32_BusConnectionDataIndex =
                              static_cast<uint32_t>(s32_BusConnectionDataIndex);
                           const C_PuiSdNode * const pc_UiNode =
                              C_PuiSdHandler::h_GetInstance()->GetUiNode(u32_NodeIndex);
                           //Core
                           const C_PuiSdNodeConnectionId * const pc_CurConnId = pc_Node->GetNodeConnectionId(
                              pc_BusConnector);
                           if (pc_CurConnId != NULL)
                           {
                              rc_CurBusConnectionBackupData.c_ConnectionId = *pc_CurConnId;
                           }
                           //UI
                           if (pc_UiNode != NULL)
                           {
                              if (u32_BusConnectionDataIndex < pc_UiNode->c_UiBusConnections.size())
                              {
                                 rc_CurBusConnectionBackupData.c_UiData =
                                    pc_UiNode->c_UiBusConnections[u32_BusConnectionDataIndex];
                              }
                           }
                        }
                     }
                  }
                  //Bus data
                  pc_BusConst = pc_BusConnector->GetBusItem();
                  if (pc_BusConst != NULL)
                  {
                     rc_CurBusConnectionBackupData.u64_BusId = pc_BusConst->GetId();
                  }
               }
            }
         }
      }
   }
   //Clear node connectors to avoid duplicate creation
   for (uint32_t u32_It = 0; u32_It < this->mc_DataBackup.c_UiNodes.size(); ++u32_It)
   {
      C_OscNode & rc_OscNode = this->mc_DataBackup.c_OscNodes[u32_It];
      C_PuiSdNode & rc_UiNode = this->mc_DataBackup.c_UiNodes[u32_It];
      for (uint32_t u32_ItComInterface = 0; u32_ItComInterface < rc_OscNode.c_Properties.c_ComInterfaces.size();
           ++u32_ItComInterface)
      {
         C_OscNodeComInterfaceSettings & rc_CurInterface =
            rc_OscNode.c_Properties.c_ComInterfaces[u32_ItComInterface];
         rc_CurInterface.RemoveConnection();
      }
      rc_UiNode.c_UiBusConnections.clear();
   }
   //Adapt text element ids
   for (uint32_t u32_ItBusTextElement = 0; u32_ItBusTextElement < this->mc_DataBackup.c_BusTextElements.size();
        ++u32_ItBusTextElement)
   {
      C_PuiSdTextElementBus & rc_BusTextElement = this->mc_DataBackup.c_BusTextElements[u32_ItBusTextElement];
      rc_BusTextElement.u32_BusIndex = c_MapOldIndexToNewIndex.value(rc_BusTextElement.u32_BusIndex,
                                                                     std::numeric_limits<uint32_t>::max());
      tgl_assert(rc_BusTextElement.u32_BusIndex != std::numeric_limits<uint32_t>::max());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save to data, node

   \param[in]   opc_UiNodeItem      UI node item
   \param[in]   ou32_NodeIndex      Node index
   \param[in]   ou64_UniqueId       Unique ID
   \param[out]  oru32_NodeCounter   Node counter
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyAddDeleteBaseCommand::m_SaveToDataNode(const C_GiNode * const opc_UiNodeItem,
                                                              const uint32_t ou32_NodeIndex,
                                                              const uint64_t ou64_UniqueId,
                                                              uint32_t & oru32_NodeCounter)
{
   if (opc_UiNodeItem != NULL)
   {
      const bool q_IsMulti = C_OscNodeSquad::h_CheckIsMultiDevice(ou32_NodeIndex,
                                                                  C_PuiSdHandler::h_GetInstance()->GetOscSystemDefinitionConst().c_NodeSquads);
      if (q_IsMulti)
      {
         this->m_SaveToDataMultiNode(ou32_NodeIndex);
      }
      else
      {
         const C_OscNode * const pc_OscNode =
            C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(ou32_NodeIndex);
         const C_PuiSdNode * const pc_UiNode =
            C_PuiSdHandler::h_GetInstance()->GetUiNode(ou32_NodeIndex);
         if (pc_OscNode != NULL)
         {
            this->mc_DataBackup.c_OscNodes.push_back(*pc_OscNode);
         }
         if (pc_UiNode != NULL)
         {
            this->mc_DataBackup.c_UiNodes.push_back(*pc_UiNode);
         }
      }
      if (this->mc_DataBackup.c_OscNodes.size() == this->mc_DataBackup.c_UiNodes.size())
      {
         this->mc_MapTypeAndIndexToId.insert(C_PuiBsTemporaryDataId(static_cast<int32_t>(
                                                                       C_PuiSdDataElement::
                                                                       E_Type::eNODE), oru32_NodeCounter),
                                             ou64_UniqueId);
      }
      ++oru32_NodeCounter;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save to data, multi node

   \param[in]  ou32_NodeIndex    Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyAddDeleteBaseCommand::m_SaveToDataMultiNode(const uint32_t ou32_NodeIndex)
{
   uint32_t u32_GroupIndex;

   if (C_PuiSdHandler::h_GetInstance()->GetNodeSquadIndexWithNodeIndex(ou32_NodeIndex,
                                                                       u32_GroupIndex) == C_NO_ERR)
   {
      const C_OscNodeSquad * const pc_Group = C_PuiSdHandler::h_GetInstance()->GetOscNodeSquadConst(
         u32_GroupIndex);
      if (pc_Group != NULL)
      {
         const std::vector<uint32_t> c_Indices = C_PuiSdHandler::h_GetInstance()->GetAllNodeGroupIndicesUsingNodeIndex(
            ou32_NodeIndex);
         C_OscNodeSquad c_Group = *pc_Group;
         c_Group.c_SubNodeIndexes.clear();
         c_Group.c_SubNodeIndexes.reserve(c_Indices.size());
         for (uint32_t u32_ItNode = 0UL; u32_ItNode < c_Indices.size(); ++u32_ItNode)
         {
            const C_OscNode * const pc_OscNode =
               C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(c_Indices[u32_ItNode]);
            const C_PuiSdNode * const pc_UiNode =
               C_PuiSdHandler::h_GetInstance()->GetUiNode(c_Indices[u32_ItNode]);
            c_Group.c_SubNodeIndexes.push_back(this->mc_DataBackup.c_OscNodes.size());
            if (pc_OscNode != NULL)
            {
               this->mc_DataBackup.c_OscNodes.push_back(*pc_OscNode);
            }
            if (pc_UiNode != NULL)
            {
               this->mc_DataBackup.c_UiNodes.push_back(*pc_UiNode);
            }
         }
         this->mc_DataBackup.c_OscNodeGroups.push_back(c_Group);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clear all data storage
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyAddDeleteBaseCommand::m_Clear(void)
{
   this->mc_MapTypeAndIndexToId.clear();
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle CANopen data before delete
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyAddDeleteBaseCommand::m_HandleCanOpenBeforeDelete() const
{
   const vector<QGraphicsItem *> c_RelatedItems = this->m_GetSceneItems();
   C_GiNode * pc_Node;
   C_GiLiBus * pc_Bus;
   C_GiLiBusConnector * pc_BusConnector;
   C_PuiSdDataElement * pc_Data;

   //Other elements
   for (vector<QGraphicsItem *>::const_iterator c_ItRelatedItem = c_RelatedItems.begin();
        c_ItRelatedItem != c_RelatedItems.end(); ++c_ItRelatedItem)
   {
      C_GiUnique * const pc_Unique = dynamic_cast<C_GiUnique *>(*c_ItRelatedItem);
      if (pc_Unique != NULL)
      {
         //lint -e{740}  no problem because of common base class

         pc_Data = dynamic_cast<C_PuiSdDataElement *>(*c_ItRelatedItem);
         if (pc_Data != NULL)
         {
            const int32_t s32_Index = pc_Data->GetIndex();
            if (s32_Index >= 0)
            {
               const uint32_t u32_Index = static_cast<uint32_t>(s32_Index);
               //Save to scene data
               pc_Data->UpdateData();

               //Backup scene data internally
               //Node
               if (dynamic_cast<C_GiNode *>(*c_ItRelatedItem) != NULL)
               {
                  m_HandleCanOpenNodeBeforeDelete(u32_Index);
               }
               //Bus

               pc_Bus = dynamic_cast<C_GiLiBus *>(*c_ItRelatedItem);
               if (pc_Bus != NULL)
               {
                  m_HandleCanOpenBusBeforeDelete(u32_Index);
               }
            }
         }
         else
         {
            //Bus connector

            pc_BusConnector = dynamic_cast<C_GiLiBusConnector *>(*c_ItRelatedItem);
            if (pc_BusConnector != NULL)
            {
               //Node data
               pc_Node = pc_BusConnector->GetNodeItem();
               if (pc_Node != NULL)
               {
                  const int32_t s32_NodeIndex = pc_Node->GetIndex();

                  //Trigger update node data again to be save in the case only the bus of a bus connector was
                  // selected
                  pc_Node->UpdateData();
                  if (s32_NodeIndex >= 0)
                  {
                     const uint32_t u32_NodeIndex = static_cast<uint32_t>(s32_NodeIndex);
                     const int32_t s32_BusConnectionDataIndex = pc_Node->GetIndexOfConnector(pc_BusConnector);
                     if (s32_BusConnectionDataIndex >= 0)
                     {
                        //Core
                        const C_PuiSdNodeConnectionId * const pc_CurConnId = pc_Node->GetNodeConnectionId(
                           pc_BusConnector);
                        if (pc_CurConnId != NULL)
                        {
                           m_HandleCanOpenNodeBusConnectorBeforeDelete(u32_NodeIndex, *pc_CurConnId);
                        }
                     }
                  }
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle CANopen node data before delete

   \param[in]  ou32_Index  Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyAddDeleteBaseCommand::m_HandleCanOpenNodeBeforeDelete(const uint32_t ou32_Index) const
{
   m_HandleCanOpenManagerBeforeDelete(ou32_Index);
   m_HandleCanOpenDeviceBeforeDelete(ou32_Index);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle CANopen manager data before delete

   \param[in]  ou32_Index  Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyAddDeleteBaseCommand::m_HandleCanOpenManagerBeforeDelete(const uint32_t ou32_Index) const
{
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(ou32_Index);

   if (pc_Node != NULL)
   {
      std::vector<uint8_t> c_Tmp;
      c_Tmp.reserve(pc_Node->c_CanOpenManagers.size());
      for (std::map<uint8_t, C_OscCanOpenManagerInfo>::const_iterator c_ItManager =
              pc_Node->c_CanOpenManagers.begin();
           c_ItManager != pc_Node->c_CanOpenManagers.end(); ++c_ItManager)
      {
         c_Tmp.push_back(c_ItManager->first);
      }
      for (uint32_t u32_ItDelete = 0UL; u32_ItDelete < c_Tmp.size(); ++u32_ItDelete)
      {
         bool q_Tmp;
         tgl_assert(C_PuiSdHandler::h_GetInstance()->DeleteCanOpenManager(ou32_Index,
                                                                          c_Tmp[u32_ItDelete], true,
                                                                          q_Tmp) == C_NO_ERR);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle CANopen device data before delete

   \param[in]  ou32_Index  Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyAddDeleteBaseCommand::m_HandleCanOpenDeviceBeforeDelete(const uint32_t ou32_Index) const
{
   for (uint32_t u32_ItNode = 0UL; u32_ItNode < C_PuiSdHandler::h_GetInstance()->GetOscNodesSize(); ++u32_ItNode)
   {
      const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(u32_ItNode);
      if (pc_Node != NULL)
      {
         for (std::map<uint8_t, C_OscCanOpenManagerInfo>::const_iterator c_ItManager =
                 pc_Node->c_CanOpenManagers.begin();
              c_ItManager != pc_Node->c_CanOpenManagers.end(); ++c_ItManager)
         {
            std::vector<C_OscCanInterfaceId> c_Tmp;
            for (std::map<C_OscCanInterfaceId, C_OscCanOpenManagerDeviceInfo>::const_iterator c_ItDevice =
                    c_ItManager->second.c_CanOpenDevices.begin();
                 c_ItDevice != c_ItManager->second.c_CanOpenDevices.end(); ++c_ItDevice)
            {
               if (c_ItDevice->first.u32_NodeIndex == ou32_Index)
               {
                  c_Tmp.push_back(c_ItDevice->first);
               }
            }
            for (uint32_t u32_ItDelete = 0UL; u32_ItDelete < c_Tmp.size(); ++u32_ItDelete)
            {
               tgl_assert(C_PuiSdHandler::h_GetInstance()->DeleteCanOpenManagerDevice(u32_ItNode, c_ItManager->first,
                                                                                      c_Tmp[u32_ItDelete]) == C_NO_ERR);
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle CANopen bus data before delete

   \param[in]  ou32_Index  Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyAddDeleteBaseCommand::m_HandleCanOpenBusBeforeDelete(const uint32_t ou32_Index) const
{
   const C_OscSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOscBus(ou32_Index);

   if (pc_Bus != NULL)
   {
      if (pc_Bus->e_Type == C_OscSystemBus::eCAN)
      {
         std::vector<uint32_t> c_NodeIndexes;
         std::vector<uint32_t> c_InterfaceIndexes;
         C_PuiSdHandler::h_GetInstance()->GetOscSystemDefinitionConst().GetNodeIndexesOfBus(ou32_Index, c_NodeIndexes,
                                                                                            c_InterfaceIndexes);
         tgl_assert(c_NodeIndexes.size() == c_InterfaceIndexes.size());
         if (c_NodeIndexes.size() == c_InterfaceIndexes.size())
         {
            for (uint32_t u32_ItNode = 0UL; u32_ItNode < c_NodeIndexes.size(); ++u32_ItNode)
            {
               uint8_t u8_InterfaceNumber;
               tgl_assert(C_PuiSdHandler::h_GetInstance()->TranslateCanInterfaceIndexToId(c_NodeIndexes[u32_ItNode],
                                                                                          c_InterfaceIndexes[u32_ItNode],
                                                                                          u8_InterfaceNumber) ==
                          C_NO_ERR);
               //Don't check result as this manager might not exist
               C_PuiSdHandler::h_GetInstance()->DeleteAllCanOpenManagerDevices(c_NodeIndexes[u32_ItNode],
                                                                               u8_InterfaceNumber);
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle CANopen node bus connector data before delete

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  orc_ConnectionId  Connection ID
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyAddDeleteBaseCommand::m_HandleCanOpenNodeBusConnectorBeforeDelete(const uint32_t ou32_NodeIndex,
                                                                                         const C_PuiSdNodeConnectionId & orc_ConnectionId)
const
{
   if (orc_ConnectionId.e_InterfaceType == C_OscSystemBus::eCAN)
   {
      m_HandleCanOpenManagerNodeBusConnectorBeforeDelete(ou32_NodeIndex, orc_ConnectionId);
      m_HandleCanOpenDeviceNodeBusConnectorBeforeDelete(ou32_NodeIndex, orc_ConnectionId);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle CANopen manager node bus connector data before delete

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  orc_ConnectionId  Connection ID
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyAddDeleteBaseCommand::m_HandleCanOpenManagerNodeBusConnectorBeforeDelete(
   const uint32_t ou32_NodeIndex, const C_PuiSdNodeConnectionId & orc_ConnectionId) const
{
   //Don't check result as this manager might not exist
   C_PuiSdHandler::h_GetInstance()->DeleteAllCanOpenManagerDevices(ou32_NodeIndex, orc_ConnectionId.u8_InterfaceNumber);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle CANopen device node bus connector data before delete

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  orc_ConnectionId  Connection ID
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyAddDeleteBaseCommand::m_HandleCanOpenDeviceNodeBusConnectorBeforeDelete(
   const uint32_t ou32_NodeIndex, const C_PuiSdNodeConnectionId & orc_ConnectionId) const
{
   for (uint32_t u32_ItNode = 0UL; u32_ItNode < C_PuiSdHandler::h_GetInstance()->GetOscNodesSize(); ++u32_ItNode)
   {
      const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(u32_ItNode);
      if (pc_Node != NULL)
      {
         for (std::map<uint8_t, C_OscCanOpenManagerInfo>::const_iterator c_ItManager =
                 pc_Node->c_CanOpenManagers.begin();
              c_ItManager != pc_Node->c_CanOpenManagers.end(); ++c_ItManager)
         {
            std::vector<C_OscCanInterfaceId> c_Tmp;
            for (std::map<C_OscCanInterfaceId, C_OscCanOpenManagerDeviceInfo>::const_iterator c_ItDevice =
                    c_ItManager->second.c_CanOpenDevices.begin();
                 c_ItDevice != c_ItManager->second.c_CanOpenDevices.end(); ++c_ItDevice)
            {
               if ((c_ItDevice->first.u32_NodeIndex == ou32_NodeIndex) &&
                   (c_ItDevice->first.u8_InterfaceNumber == orc_ConnectionId.u8_InterfaceNumber))
               {
                  c_Tmp.push_back(c_ItDevice->first);
               }
            }
            for (uint32_t u32_ItDelete = 0UL; u32_ItDelete < c_Tmp.size(); ++u32_ItDelete)
            {
               tgl_assert(C_PuiSdHandler::h_GetInstance()->DeleteCanOpenManagerDevice(u32_ItNode, c_ItManager->first,
                                                                                      c_Tmp[u32_ItDelete]) == C_NO_ERR);
            }
         }
      }
   }
}
