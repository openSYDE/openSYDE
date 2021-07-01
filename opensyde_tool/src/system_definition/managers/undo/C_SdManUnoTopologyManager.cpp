//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handler class for topology undo command stack (implementation)

   Handler class for topology undo command stack

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_SdManUnoTopologyManager.h"
#include "C_GiUnique.h"
#include "C_SdManUnoTopologySetupStyleCommand.h"
#include "C_SdManUnoTopologyZOrderCommand.h"
#include "C_SdManUnoTopologyDeleteCommand.h"
#include "C_SdManUnoTopologyAddSnapshotCommand.h"
#include "C_SdManUnoTopologyReconnectNodeCommand.h"
#include "C_SdManUnoTopologyReconnectBusCommand.h"
#include "C_SdManUnoTopologyChangeInterfaceCommand.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace std;
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

   \param[in,out]  opc_Scene     Scene to use undo redo framework for
   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdManUnoTopologyManager::C_SdManUnoTopologyManager(QGraphicsScene * const opc_Scene, QObject * const opc_Parent) :
   C_SebUnoTopBaseManager(opc_Scene, opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdManUnoTopologyManager::~C_SdManUnoTopologyManager(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt z order of selected items

   With check for z order priority

   \param[in]  orc_SelectedItems    All selected items
   \param[in]  orc_Items            All items
   \param[in]  oq_BringToFront      Flag if this is the bring to front action (otherwise send to back assumed)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyManager::AdaptZOrder(const QList<QGraphicsItem *> & orc_SelectedItems,
                                            const QList<QGraphicsItem *> & orc_Items, const bool oq_BringToFront)
{
   QMap<QGraphicsItem *, float64> c_Changes;
   vector<uint64> c_IDs;
   vector<float64> c_Values;
   QList<QGraphicsItem *> c_List;
   C_SdManUnoTopologyZOrderCommand * pc_ZOrderCommand;

   C_SdManUnoTopologyZOrderCommand::h_AdaptZOrder(this->mpc_Scene, orc_Items, orc_SelectedItems, oq_BringToFront,
                                                  c_Changes);
   C_SdManUnoTopologyZOrderCommand::h_CheckZOrderPriority(orc_SelectedItems, orc_Items, c_Changes);

   for (QMap<QGraphicsItem *, float64>::const_iterator c_ItChanges = c_Changes.begin();
        c_ItChanges != c_Changes.end();
        ++c_ItChanges)
   {
      c_List.push_back(c_ItChanges.key());
      c_Values.push_back(c_ItChanges.value());
   }
   mh_MapItemToID(c_List, c_IDs);
   pc_ZOrderCommand = new C_SdManUnoTopologyZOrderCommand(this->mpc_Scene, c_IDs, c_Values);
   this->DoPush(pc_ZOrderCommand);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do delete

   \param[in,out]  orc_Items  Items to delete
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyManager::DoDelete(const QList<QGraphicsItem *> & orc_Items)
{
   if (orc_Items.size() > 0)
   {
      vector<uint64> c_IDs;
      C_SdManUnoTopologyDeleteCommand * pc_DeleteCommand;
      QList<QGraphicsItem *> c_ImprovedItemList;

      if (this->mpc_Scene != NULL)
      {
         //Special handling if possible to include all affected items, not only the selected ones
         C_SdManUnoTopologyAddDeleteBaseCommand::h_GetAllRelevantObjects(
            this->mpc_Scene->items(), orc_Items, c_ImprovedItemList);
         mh_MapItemToID(c_ImprovedItemList, c_IDs);
      }
      else
      {
         mh_MapItemToID(orc_Items, c_IDs);
      }
      pc_DeleteCommand = new C_SdManUnoTopologyDeleteCommand(this->mpc_Scene, c_IDs);
      connect(pc_DeleteCommand,
              &C_SdManUnoTopologyAddDeleteBaseCommand::SigErrorChange, this,
              &C_SdManUnoTopologyManager::m_OnErrorChange);
      this->m_DoPushAndSignalError(pc_DeleteCommand);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add one element

   \param[in]  ore_Type                   Type
   \param[in]  oru64_UniqueID             Unique ID
   \param[in]  orc_NewPos                 Position
   \param[in]  orc_AdditionalInformation  Additional string information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyManager::DoAddGeneric(const C_SdManUnoTopologyAddCommand::E_ElementType & ore_Type,
                                             const uint64 & oru64_UniqueID, const QPointF & orc_NewPos,
                                             const QString & orc_AdditionalInformation)
{
   vector<uint64> c_IDs;
   C_SdManUnoTopologyAddCommand * pc_AddCommand;

   c_IDs.push_back(oru64_UniqueID);
   pc_AddCommand = new C_SdManUnoTopologyAddCommand(this->mpc_Scene, c_IDs, ore_Type, orc_NewPos,
                                                    orc_AdditionalInformation);
   connect(pc_AddCommand,
           &C_SdManUnoTopologyAddDeleteBaseCommand::SigErrorChange, this, &C_SdManUnoTopologyManager::m_OnErrorChange);
   this->DoPush(pc_AddCommand);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add one element

   \param[in]  ore_Type                Type
   \param[in]  oru64_UniqueIDBus       Unique ID for bus element
   \param[in]  oru64_UniqueIDBusName   Unique ID for bus name text element
   \param[in]  orc_NewPos              Position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyManager::DoAddBus(const C_SdManUnoTopologyAddCommand::E_ElementType & ore_Type,
                                         const uint64 & oru64_UniqueIDBus, const uint64 & oru64_UniqueIDBusName,
                                         const QPointF & orc_NewPos)
{
   vector<uint64> c_IDs;
   C_SdManUnoTopologyAddCommand * pc_AddCommand;

   c_IDs.push_back(oru64_UniqueIDBus);
   c_IDs.push_back(oru64_UniqueIDBusName);
   pc_AddCommand = new C_SdManUnoTopologyAddCommand(this->mpc_Scene, c_IDs, ore_Type, orc_NewPos, "");

   connect(pc_AddCommand,
           &C_SdManUnoTopologyAddDeleteBaseCommand::SigErrorChange, this, &C_SdManUnoTopologyManager::m_OnErrorChange);
   this->DoPush(pc_AddCommand);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add one bus connector element

   \param[in]  oru64_UniqueID             Unique ID
   \param[in]  orc_NewPos                 Position
   \param[in]  opc_Node                   Node for bus connector creation
   \param[in]  opc_Bus                    Bus for bus connector creation
   \param[in]  oru8_InterfaceNumber       Interface number for bus connector creation
   \param[in]  oru8_NodeId                New node id
   \param[in]  oq_ActivateDatapoolL2      Activate datapool L2
   \param[in]  oq_ActivateDatapoolECeS    Activate datapool ECeS
   \param[in]  oq_ActivateDatapoolECoS    Activate datapool ECoS
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyManager::DoAddBusConnector(const uint64 & oru64_UniqueID, const QPointF & orc_NewPos,
                                                  const QGraphicsItem * const opc_Node,
                                                  const QGraphicsItem * const opc_Bus,
                                                  const stw_types::uint8 & oru8_InterfaceNumber,
                                                  const stw_types::uint8 & oru8_NodeId,
                                                  const bool oq_ActivateDatapoolL2, const bool oq_ActivateDatapoolECeS,
                                                  const bool oq_ActivateDatapoolECoS)
{
   vector<uint64> c_IDs;
   uint64 u64_NodeID;
   uint64 u64_BusID;
   C_SdManUnoTopologyAddCommand * pc_AddCommand;

   c_IDs.push_back(oru64_UniqueID);
   mh_MapItemToID(opc_Node, u64_NodeID);
   mh_MapItemToID(opc_Bus, u64_BusID);
   pc_AddCommand = new C_SdManUnoTopologyAddCommand(this->mpc_Scene, c_IDs,
                                                    C_SdManUnoTopologyAddCommand::E_ElementType::eBUS_CONNECTOR,
                                                    orc_NewPos,
                                                    u64_NodeID, u64_BusID, oru8_InterfaceNumber, oru8_NodeId,
                                                    oq_ActivateDatapoolL2, oq_ActivateDatapoolECeS,
                                                    oq_ActivateDatapoolECoS);
   connect(pc_AddCommand,
           &C_SdManUnoTopologyAddDeleteBaseCommand::SigErrorChange, this, &C_SdManUnoTopologyManager::m_OnErrorChange);
   this->m_DoPushAndSignalError(pc_AddCommand);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do add one node to node connection and create a new bus

   \param[in]  orc_FourUniqueIds             Four unique IDs for the four new items created by this action
   \param[in]  ore_BusType                   Bus type to create
   \param[in]  orc_BusName                   Bus name to use initially
   \param[in]  orc_BusPosition               Target bus position
   \param[in]  oru64_Node1UniqueID           Node 1 unique ID
   \param[in]  oru64_Node2UniqueID           Node 2 unique ID
   \param[in]  oru8_Node1InterfaceNumber     Node 1 interface number
   \param[in]  oru8_Node2InterfaceNumber     Node 2 interface number
   \param[in]  oru8_Node1NodeID              Node 1 node ID
   \param[in]  oru8_Node2NodeID              Node 2 node ID
   \param[in]  oq_Node1ActivateDatapoolL2    Node1 activate datapool L2
   \param[in]  oq_Node1ActivateDatapoolECeS  Node1 activate datapool ECeS
   \param[in]  oq_Node1ActivateDatapoolECoS  Node1 activate datapool ECoS
   \param[in]  oq_Node2ActivateDatapoolL2    Node2 activate datapool L2
   \param[in]  oq_Node2ActivateDatapoolECeS  Node2 activate datapool ECeS
   \param[in]  oq_Node2ActivateDatapoolECoS  Node2 activate datapool ECoS
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyManager::DoAddNodeToNodeConnectionAndCreateNewBus(const std::vector<uint64> & orc_FourUniqueIds, const stw_opensyde_core::C_OSCSystemBus::E_Type
                                                                         & ore_BusType, const QString & orc_BusName,
                                                                         const QPointF & orc_BusPosition,
                                                                         const uint64 & oru64_Node1UniqueID,
                                                                         const uint64 & oru64_Node2UniqueID,
                                                                         const uint8 & oru8_Node1InterfaceNumber,
                                                                         const uint8 & oru8_Node2InterfaceNumber,
                                                                         const uint8 & oru8_Node1NodeID,
                                                                         const uint8 & oru8_Node2NodeID,
                                                                         const bool oq_Node1ActivateDatapoolL2,
                                                                         const bool oq_Node1ActivateDatapoolECeS,
                                                                         const bool oq_Node1ActivateDatapoolECoS,
                                                                         const bool oq_Node2ActivateDatapoolL2,
                                                                         const bool oq_Node2ActivateDatapoolECeS,
                                                                         const bool oq_Node2ActivateDatapoolECoS)
{
   if (orc_FourUniqueIds.size() >= 4)
   {
      QUndoCommand * const pc_UndoCommand = new QUndoCommand("Add node to node connection (creating new bus)");
      C_SdManUnoTopologyAddCommand * pc_TmpAddCmd;
      vector<uint64> c_IDs;
      C_SdManUnoTopologyAddCommand::E_ElementType e_ElementType;
      if (ore_BusType == stw_opensyde_core::C_OSCSystemBus::eETHERNET)
      {
         e_ElementType = C_SdManUnoTopologyAddCommand::eETHERNET_BUS;
      }
      else
      {
         e_ElementType = C_SdManUnoTopologyAddCommand::eCAN_BUS;
      }

      c_IDs.push_back(orc_FourUniqueIds[0]);
      c_IDs.push_back(orc_FourUniqueIds[1]);
      pc_TmpAddCmd = new C_SdManUnoTopologyAddCommand(this->mpc_Scene, c_IDs, e_ElementType, orc_BusPosition,
                                                      orc_BusName,
                                                      pc_UndoCommand, true);
      connect(pc_TmpAddCmd,
              &C_SdManUnoTopologyAddDeleteBaseCommand::SigErrorChange, this,
              &C_SdManUnoTopologyManager::m_OnErrorChange);
      c_IDs.clear();
      c_IDs.push_back(orc_FourUniqueIds[2]);
      //lint -e{423}  //no memory leak because of Qt memory management and redo engine handling
      pc_TmpAddCmd = new C_SdManUnoTopologyAddCommand(this->mpc_Scene, c_IDs,
                                                      C_SdManUnoTopologyAddCommand::eBUS_CONNECTOR,
                                                      orc_BusPosition,
                                                      oru64_Node1UniqueID, orc_FourUniqueIds[0],
                                                      oru8_Node1InterfaceNumber,
                                                      oru8_Node1NodeID, oq_Node1ActivateDatapoolL2,
                                                      oq_Node1ActivateDatapoolECeS, oq_Node1ActivateDatapoolECoS,
                                                      pc_UndoCommand);
      connect(pc_TmpAddCmd,
              &C_SdManUnoTopologyAddDeleteBaseCommand::SigErrorChange, this,
              &C_SdManUnoTopologyManager::m_OnErrorChange);
      c_IDs.clear();
      c_IDs.push_back(orc_FourUniqueIds[3]);
      //lint -e{423}  //no memory leak because of Qt memory management and redo engine handling
      pc_TmpAddCmd = new C_SdManUnoTopologyAddCommand(this->mpc_Scene, c_IDs,
                                                      C_SdManUnoTopologyAddCommand::eBUS_CONNECTOR,
                                                      orc_BusPosition,
                                                      oru64_Node2UniqueID, orc_FourUniqueIds[0],
                                                      oru8_Node2InterfaceNumber,
                                                      oru8_Node2NodeID, oq_Node2ActivateDatapoolL2,
                                                      oq_Node2ActivateDatapoolECeS, oq_Node2ActivateDatapoolECoS,
                                                      pc_UndoCommand);
      connect(pc_TmpAddCmd,
              &C_SdManUnoTopologyAddDeleteBaseCommand::SigErrorChange, this,
              &C_SdManUnoTopologyManager::m_OnErrorChange);
      this->m_DoPushAndSignalError(pc_UndoCommand);
   } //lint !e429  //no memory leak because of Qt memory management and redo engine handling
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do add one node to node connection and use an existing bus

   \param[in]  orc_TwoUniqueIds              Two unique IDs for the two new items created by this action
   \param[in]  oru64_BusUniqueID             Bus unique ID
   \param[in]  orc_Node1Position             Node 1 position
   \param[in]  orc_Node2Position             Node 2 position
   \param[in]  oru64_Node1UniqueID           Node 1 unique ID
   \param[in]  oru64_Node2UniqueID           Node 2 unique ID
   \param[in]  oru8_Node1InterfaceNumber     Node 1 interface number
   \param[in]  oru8_Node2InterfaceNumber     Node 2 interface number
   \param[in]  oru8_Node1NodeID              Node 1 node ID
   \param[in]  oru8_Node2NodeID              Node 2 node ID
   \param[in]  oq_Node1ActivateDatapoolL2    Node1 activate datapool L2
   \param[in]  oq_Node1ActivateDatapoolECeS  Node1 activate datapool ECeS
   \param[in]  oq_Node1ActivateDatapoolECoS  Node1 activate datapool ECoS
   \param[in]  oq_Node2ActivateDatapoolL2    Node2 activate datapool L2
   \param[in]  oq_Node2ActivateDatapoolECeS  Node2 activate datapool ECeS
   \param[in]  oq_Node2ActivateDatapoolECoS  Node2 activate datapool ECoS
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyManager::DoAddNodeToNodeConnectionUsingExistingBus(const std::vector<uint64> & orc_TwoUniqueIds,
                                                                          const uint64 & oru64_BusUniqueID,
                                                                          const QPointF & orc_Node1Position,
                                                                          const QPointF & orc_Node2Position,
                                                                          const uint64 & oru64_Node1UniqueID,
                                                                          const uint64 & oru64_Node2UniqueID,
                                                                          const uint8 & oru8_Node1InterfaceNumber,
                                                                          const uint8 & oru8_Node2InterfaceNumber,
                                                                          const uint8 & oru8_Node1NodeID,
                                                                          const uint8 & oru8_Node2NodeID,
                                                                          const bool oq_Node1ActivateDatapoolL2,
                                                                          const bool oq_Node1ActivateDatapoolECeS,
                                                                          const bool oq_Node1ActivateDatapoolECoS,
                                                                          const bool oq_Node2ActivateDatapoolL2,
                                                                          const bool oq_Node2ActivateDatapoolECeS,
                                                                          const bool oq_Node2ActivateDatapoolECoS)
{
   if (orc_TwoUniqueIds.size() >= 2)
   {
      QUndoCommand * const pc_UndoCommand = new QUndoCommand("Add node to node connection (using existing bus)");
      vector<uint64> c_IDs;
      C_SdManUnoTopologyAddCommand * pc_TmpAddCmd;
      c_IDs.clear();
      c_IDs.push_back(orc_TwoUniqueIds[0]);
      //lint -e{423}  //no memory leak because of Qt memory management and redo engine handling
      pc_TmpAddCmd = new C_SdManUnoTopologyAddCommand(this->mpc_Scene, c_IDs,
                                                      C_SdManUnoTopologyAddCommand::eBUS_CONNECTOR,
                                                      orc_Node1Position, oru64_Node1UniqueID, oru64_BusUniqueID,
                                                      oru8_Node1InterfaceNumber, oru8_Node1NodeID,
                                                      oq_Node1ActivateDatapoolL2,
                                                      oq_Node1ActivateDatapoolECeS, oq_Node1ActivateDatapoolECoS,
                                                      pc_UndoCommand);
      connect(pc_TmpAddCmd,
              &C_SdManUnoTopologyAddDeleteBaseCommand::SigErrorChange, this,
              &C_SdManUnoTopologyManager::m_OnErrorChange);
      c_IDs.clear();
      c_IDs.push_back(orc_TwoUniqueIds[1]);
      //lint -e{423}  //no memory leak because of Qt memory management and redo engine handling
      pc_TmpAddCmd = new C_SdManUnoTopologyAddCommand(this->mpc_Scene, c_IDs,
                                                      C_SdManUnoTopologyAddCommand::eBUS_CONNECTOR,
                                                      orc_Node2Position, oru64_Node2UniqueID, oru64_BusUniqueID,
                                                      oru8_Node2InterfaceNumber, oru8_Node2NodeID,
                                                      oq_Node2ActivateDatapoolL2,
                                                      oq_Node2ActivateDatapoolECeS, oq_Node2ActivateDatapoolECoS,
                                                      pc_UndoCommand);
      connect(pc_TmpAddCmd,
              &C_SdManUnoTopologyAddDeleteBaseCommand::SigErrorChange, this,
              &C_SdManUnoTopologyManager::m_OnErrorChange);
      this->m_DoPushAndSignalError(pc_UndoCommand);
   } //lint !e429  //no memory leak because of Qt memory management and redo engine handling
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new data based on a snapshot and reserved IDs

   \param[in]  oru64_UniqueIDs         Reserved unique IDs for snapshot data
   \param[in]  orc_Snapshot            Snapshot data
   \param[in]  orc_NewPos              Position offset for all items
   \param[in]  of64_HighestUsedZValue  Highest used Z value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyManager::DoAddSnapshot(const std::vector<uint64> & oru64_UniqueIDs,
                                              const C_SdTopologyDataSnapshot & orc_Snapshot, const QPointF & orc_NewPos,
                                              const float64 of64_HighestUsedZValue)
{
   C_SdManUnoTopologyAddSnapshotCommand * pc_AddCommand;

   pc_AddCommand = new C_SdManUnoTopologyAddSnapshotCommand(this->mpc_Scene, orc_Snapshot,
                                                            oru64_UniqueIDs, orc_NewPos, of64_HighestUsedZValue);
   connect(pc_AddCommand,
           &C_SdManUnoTopologyAddDeleteBaseCommand::SigErrorChange, this, &C_SdManUnoTopologyManager::m_OnErrorChange);
   this->DoPush(pc_AddCommand);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reconnect to different node

   \param[in]  opc_BusConnector           Bus connector to reconnect
   \param[in]  opc_StartingNode           Initial node
   \param[in]  opc_LastNode               New node
   \param[in]  orc_ConnectionPos          Event position
   \param[in]  ors32_Interface            Interface to connect to
   \param[in]  oru8_NodeId                New node id
   \param[in]  oq_ActivateDatapoolL2      Activate datapool L2
   \param[in]  oq_ActivateDatapoolECeS    Activate datapool ECeS
   \param[in]  oq_ActivateDatapoolECoS    Activate datapool E co S
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyManager::DoReconnectNode(const C_GiLiBusConnector * const opc_BusConnector,
                                                const C_GiNode * const opc_StartingNode,
                                                const C_GiNode * const opc_LastNode, const QPointF & orc_ConnectionPos,
                                                const sint32 & ors32_Interface, const uint8 & oru8_NodeId,
                                                const bool oq_ActivateDatapoolL2, const bool oq_ActivateDatapoolECeS,
                                                const bool oq_ActivateDatapoolECoS)
{
   if ((opc_StartingNode != NULL) && (opc_LastNode != NULL))
   {
      vector<uint64> c_IDs;
      uint64 u64_ID;
      C_SdManUnoTopologyReconnectNodeCommand * pc_ReconnectCommand;

      mh_MapItemToID(opc_BusConnector, u64_ID);
      c_IDs.push_back(u64_ID);
      pc_ReconnectCommand = new C_SdManUnoTopologyReconnectNodeCommand(this->mpc_Scene, c_IDs,
                                                                       opc_StartingNode->GetID(),
                                                                       opc_LastNode->GetID(), orc_ConnectionPos,
                                                                       ors32_Interface, oru8_NodeId,
                                                                       oq_ActivateDatapoolL2, oq_ActivateDatapoolECeS,
                                                                       oq_ActivateDatapoolECoS);
      m_MergeWithPrev(pc_ReconnectCommand);
      //Deactivate due to data change
      this->m_PrepareAction();
      //this->push(pc_ReconnectCommand);
      pc_ReconnectCommand->redo();
      Q_EMIT this->SigErrorChanged();
      this->m_CleanupAction();
      delete (pc_ReconnectCommand);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reconnect to different bus

   \param[in]  opc_BusConnector           Bus connector to reconnect
   \param[in]  opc_StartingBus            Initial bus
   \param[in]  opc_LastBus                Last bus
   \param[in]  orc_ConnectionPos          Event position
   \param[in]  ors32_Interface            Interface to connect to
   \param[in]  oru8_NodeId                New node id
   \param[in]  oq_ActivateDatapoolL2      Activate datapool L2
   \param[in]  oq_ActivateDatapoolECeS    Activate datapool ECeS
   \param[in]  oq_ActivateDatapoolECoS    Activate datapool ECoS
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyManager::DoReconnectBus(const C_GiLiBusConnector * const opc_BusConnector,
                                               const C_GiLiBus * const opc_StartingBus,
                                               const C_GiLiBus * const opc_LastBus, const QPointF & orc_ConnectionPos,
                                               const sint32 & ors32_Interface, const uint8 & oru8_NodeId,
                                               const bool oq_ActivateDatapoolL2, const bool oq_ActivateDatapoolECeS,
                                               const bool oq_ActivateDatapoolECoS)
{
   if ((opc_StartingBus != NULL) && (opc_LastBus != NULL))
   {
      vector<uint64> c_IDs;
      uint64 u64_ID;
      C_SdManUnoTopologyReconnectBusCommand * pc_ReconnectCommand;

      mh_MapItemToID(opc_BusConnector, u64_ID);
      c_IDs.push_back(u64_ID);
      pc_ReconnectCommand = new C_SdManUnoTopologyReconnectBusCommand(this->mpc_Scene, c_IDs,
                                                                      opc_StartingBus->GetID(),
                                                                      opc_LastBus->GetID(), orc_ConnectionPos,
                                                                      ors32_Interface, oru8_NodeId,
                                                                      oq_ActivateDatapoolL2, oq_ActivateDatapoolECeS,
                                                                      oq_ActivateDatapoolECoS);
      m_MergeWithPrev(pc_ReconnectCommand);
      //Deactivate due to data change
      this->m_PrepareAction();
      //this->push(pc_ReconnectCommand);
      pc_ReconnectCommand->redo();
      Q_EMIT this->SigErrorChanged();
      this->m_CleanupAction();
      delete (pc_ReconnectCommand);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Change used node interface of bus connector

   \param[in]  opc_BusConnector              Bus connector to change interface of
   \param[in]  oru8_PreviousInterfaceNumber  Last used interface number
   \param[in]  oru8_NewInterfaceNumber       New interface number to use
   \param[in]  oru8_PreviousNodeId           Previous node id
   \param[in]  oru8_NewNodeId                New node id
   \param[in]  oq_ActivateDatapoolL2         Activate datapool L2
   \param[in]  oq_ActivateDatapoolECeS       Activate datapool ECeS
   \param[in]  oq_ActivateDatapoolECoS       Activate datapool ECoS
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyManager::DoChangeInterface(const C_GiLiBusConnector * const opc_BusConnector,
                                                  const uint8 & oru8_PreviousInterfaceNumber,
                                                  const uint8 & oru8_NewInterfaceNumber,
                                                  const uint8 & oru8_PreviousNodeId, const uint8 & oru8_NewNodeId,
                                                  const bool oq_ActivateDatapoolL2, const bool oq_ActivateDatapoolECeS,
                                                  const bool oq_ActivateDatapoolECoS)
{
   if (opc_BusConnector != NULL)
   {
      vector<uint64> c_IDs;
      uint64 u64_ID;
      C_SdManUnoTopologyChangeInterfaceCommand * pc_ReconnectCommand;

      mh_MapItemToID(opc_BusConnector, u64_ID);
      c_IDs.push_back(u64_ID);
      pc_ReconnectCommand = new C_SdManUnoTopologyChangeInterfaceCommand(this->mpc_Scene, c_IDs,
                                                                         oru8_PreviousInterfaceNumber,
                                                                         oru8_NewInterfaceNumber, oru8_PreviousNodeId,
                                                                         oru8_NewNodeId, oq_ActivateDatapoolL2,
                                                                         oq_ActivateDatapoolECeS,
                                                                         oq_ActivateDatapoolECoS);
      //Deactivate due to data change
      this->m_PrepareAction();
      //this->push(pc_ReconnectCommand);
      pc_ReconnectCommand->redo();
      Q_EMIT this->SigErrorChanged();
      this->m_CleanupAction();
      delete (pc_ReconnectCommand);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get new style command (Destruction has to be handled by calling function)

   \param[in]  orc_Items      Item IDs
   \param[in]  oq_DarkMode    Optional flag if dark mode active

   \return
   Valid pointer to new style command
*/
//----------------------------------------------------------------------------------------------------------------------
C_SebUnoSetupStyleCommand * C_SdManUnoTopologyManager::m_GetNewStyleCommand(const std::vector<uint64> & orc_Items,
                                                                            const bool oq_DarkMode)
{
   return new C_SdManUnoTopologySetupStyleCommand(this->mpc_Scene, orc_Items, oq_DarkMode);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Merge specified command with previous command on stack

   \param[in,out]  opc_Command   Command to add the last command on stack to
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyManager::m_MergeWithPrev(QUndoCommand * const opc_Command)
{
   const sint32 s32_LastIndex = static_cast<sint32>(this->count()) - 1L;
   const QUndoCommand * const opc_LastCommand = this->command(s32_LastIndex);

   if (opc_LastCommand != NULL)
   {
      if (opc_Command->mergeWith(opc_LastCommand) == true)
      {
         //undo last step as this will be triggered indirectly by opc_Command
         this->undo();
      }
   }
} //lint !e429  //never took ownership of any of these objects in this function

//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyManager::m_OnErrorChange(void)
{
   Q_EMIT this->SigErrorChanged();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add newest command to undo stack and signal error

   \param[in,out]  opc_UndoCommand  New undo command to add to the stack
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyManager::m_DoPushAndSignalError(QUndoCommand * const opc_UndoCommand)
{
   this->m_PrepareAction();
   this->push(opc_UndoCommand);
   Q_EMIT this->SigErrorChanged();
   this->m_CleanupAction();
}
