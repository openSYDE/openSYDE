//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handler class for topology undo command stack (implementation)

   Handler class for topology undo command stack

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_SdManUnoTopologyManager.hpp"
#include "C_GiUnique.hpp"
#include "C_SdManUnoTopologySetupStyleCommand.hpp"
#include "C_SdManUnoTopologyZetOrderCommand.hpp"
#include "C_SdManUnoTopologyDeleteCommand.hpp"
#include "C_SdManUnoTopologyAddSnapshotCommand.hpp"
#include "C_SdManUnoTopologyReconnectNodeCommand.hpp"
#include "C_SdManUnoTopologyReconnectBusCommand.hpp"
#include "C_SdManUnoTopologyChangeInterfaceCommand.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace std;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui;

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
void C_SdManUnoTopologyManager::AdaptZetOrder(const QList<QGraphicsItem *> & orc_SelectedItems,
                                              const QList<QGraphicsItem *> & orc_Items, const bool oq_BringToFront)
{
   QMap<QGraphicsItem *, float64_t> c_Changes;
   vector<uint64_t> c_Ids;
   vector<float64_t> c_Values;
   QList<QGraphicsItem *> c_List;
   C_SdManUnoTopologyZetOrderCommand * pc_ZetOrderCommand;

   C_SdManUnoTopologyZetOrderCommand::h_AdaptZetOrder(this->mpc_Scene, orc_Items, orc_SelectedItems, oq_BringToFront,
                                                      c_Changes);
   C_SdManUnoTopologyZetOrderCommand::h_CheckZetOrderPriority(orc_SelectedItems, orc_Items, c_Changes);

   for (QMap<QGraphicsItem *, float64_t>::const_iterator c_ItChanges = c_Changes.begin();
        c_ItChanges != c_Changes.end();
        ++c_ItChanges)
   {
      c_List.push_back(c_ItChanges.key());
      c_Values.push_back(c_ItChanges.value());
   }
   mh_MapItemToId(c_List, c_Ids);
   pc_ZetOrderCommand = new C_SdManUnoTopologyZetOrderCommand(this->mpc_Scene, c_Ids, c_Values);
   this->DoPush(pc_ZetOrderCommand);
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
      vector<uint64_t> c_Ids;
      C_SdManUnoTopologyDeleteCommand * pc_DeleteCommand;
      QList<QGraphicsItem *> c_ImprovedItemList;

      if (this->mpc_Scene != NULL)
      {
         //Special handling if possible to include all affected items, not only the selected ones
         C_SdManUnoTopologyAddDeleteBaseCommand::h_GetAllRelevantObjects(
            this->mpc_Scene->items(), orc_Items, c_ImprovedItemList);
         mh_MapItemToId(c_ImprovedItemList, c_Ids);
      }
      else
      {
         mh_MapItemToId(orc_Items, c_Ids);
      }
      pc_DeleteCommand = new C_SdManUnoTopologyDeleteCommand(this->mpc_Scene, c_Ids);
      connect(pc_DeleteCommand,
              &C_SdManUnoTopologyAddDeleteBaseCommand::SigErrorChange, this,
              &C_SdManUnoTopologyManager::m_OnErrorChange);
      this->m_DoPushAndSignalError(pc_DeleteCommand);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add one element

   \param[in]  ore_Type                   Type
   \param[in]  oru64_UniqueId             Unique ID
   \param[in]  orc_NewPos                 Position
   \param[in]  orc_AdditionalInformation  Additional string information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyManager::DoAddGeneric(const C_SdManUnoTopologyAddCommand::E_ElementType & ore_Type,
                                             const uint64_t & oru64_UniqueId, const QPointF & orc_NewPos,
                                             const QString & orc_AdditionalInformation)
{
   vector<uint64_t> c_Ids;
   C_SdManUnoTopologyAddCommand * pc_AddCommand;

   c_Ids.push_back(oru64_UniqueId);
   pc_AddCommand = new C_SdManUnoTopologyAddCommand(this->mpc_Scene, c_Ids, ore_Type, orc_NewPos,
                                                    orc_AdditionalInformation);
   connect(pc_AddCommand,
           &C_SdManUnoTopologyAddDeleteBaseCommand::SigErrorChange, this, &C_SdManUnoTopologyManager::m_OnErrorChange);
   this->DoPush(pc_AddCommand);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add one element

   \param[in]  ore_Type                Type
   \param[in]  oru64_UniqueIdBus       Unique ID for bus element
   \param[in]  oru64_UniqueIdBusName   Unique ID for bus name text element
   \param[in]  orc_NewPos              Position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyManager::DoAddBus(const C_SdManUnoTopologyAddCommand::E_ElementType & ore_Type,
                                         const uint64_t & oru64_UniqueIdBus, const uint64_t & oru64_UniqueIdBusName,
                                         const QPointF & orc_NewPos)
{
   vector<uint64_t> c_Ids;
   C_SdManUnoTopologyAddCommand * pc_AddCommand;

   c_Ids.push_back(oru64_UniqueIdBus);
   c_Ids.push_back(oru64_UniqueIdBusName);
   pc_AddCommand = new C_SdManUnoTopologyAddCommand(this->mpc_Scene, c_Ids, ore_Type, orc_NewPos, "");

   connect(pc_AddCommand,
           &C_SdManUnoTopologyAddDeleteBaseCommand::SigErrorChange, this, &C_SdManUnoTopologyManager::m_OnErrorChange);
   this->DoPush(pc_AddCommand);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add one bus connector element

   \param[in]  oru64_UniqueId          Unique ID
   \param[in]  orc_NewPos              Position
   \param[in]  opc_Node                Node for bus connector creation
   \param[in]  opc_Bus                 Bus for bus connector creation
   \param[in]  oru8_InterfaceNumber    Interface number for bus connector creation
   \param[in]  orc_Properties          Properties
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyManager::DoAddBusConnector(const uint64_t & oru64_UniqueId, const QPointF & orc_NewPos,
                                                  const QGraphicsItem * const opc_Node,
                                                  const QGraphicsItem * const opc_Bus,
                                                  const uint8_t & oru8_InterfaceNumber,
                                                  const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties)
{
   vector<uint64_t> c_Ids;
   uint64_t u64_NodeId;
   uint64_t u64_BusId;
   C_SdManUnoTopologyAddCommand * pc_AddCommand;

   c_Ids.push_back(oru64_UniqueId);
   mh_MapItemToId(opc_Node, u64_NodeId);
   mh_MapItemToId(opc_Bus, u64_BusId);
   pc_AddCommand = new C_SdManUnoTopologyAddCommand(this->mpc_Scene, c_Ids,
                                                    C_SdManUnoTopologyAddCommand::E_ElementType::eBUS_CONNECTOR,
                                                    orc_NewPos,
                                                    u64_NodeId, u64_BusId, oru8_InterfaceNumber, orc_Properties);
   connect(pc_AddCommand,
           &C_SdManUnoTopologyAddDeleteBaseCommand::SigErrorChange, this, &C_SdManUnoTopologyManager::m_OnErrorChange);
   this->m_DoPushAndSignalError(pc_AddCommand);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do add one node to node connection and create a new bus

   \param[in]  orc_FourUniqueIds          Four unique IDs for the four new items created by this action
   \param[in]  ore_BusType                Bus type to create
   \param[in]  orc_BusName                Bus name to use initially
   \param[in]  orc_BusPosition            Target bus position
   \param[in]  oru64_Node1UniqueId        Node 1 unique ID
   \param[in]  oru64_Node2UniqueId        Node 2 unique ID
   \param[in]  oru8_Node1InterfaceNumber  Node 1 interface number
   \param[in]  oru8_Node2InterfaceNumber  Node 2 interface number
   \param[in]  orc_Node1Properties        Node 1 properties
   \param[in]  orc_Node2Properties        Node 2 properties
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyManager::DoAddNodeToNodeConnectionAndCreateNewBus(
   const std::vector<uint64_t> & orc_FourUniqueIds, const stw::opensyde_core::C_OscSystemBus::E_Type
   & ore_BusType, const QString & orc_BusName,
   const QPointF & orc_BusPosition, const uint64_t & oru64_Node1UniqueId, const uint64_t & oru64_Node2UniqueId,
   const uint8_t & oru8_Node1InterfaceNumber, const uint8_t & oru8_Node2InterfaceNumber,
   const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Node1Properties,
   const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Node2Properties)
{
   if (orc_FourUniqueIds.size() >= 4)
   {
      QUndoCommand * const pc_UndoCommand = new QUndoCommand("Add node to node connection (creating new bus)");
      C_SdManUnoTopologyAddCommand * pc_TmpAddCmd;
      vector<uint64_t> c_Ids;
      C_SdManUnoTopologyAddCommand::E_ElementType e_ElementType;
      if (ore_BusType == stw::opensyde_core::C_OscSystemBus::eETHERNET)
      {
         e_ElementType = C_SdManUnoTopologyAddCommand::eETHERNET_BUS;
      }
      else
      {
         e_ElementType = C_SdManUnoTopologyAddCommand::eCAN_BUS;
      }

      c_Ids.push_back(orc_FourUniqueIds[0]);
      c_Ids.push_back(orc_FourUniqueIds[1]);
      pc_TmpAddCmd = new C_SdManUnoTopologyAddCommand(this->mpc_Scene, c_Ids, e_ElementType, orc_BusPosition,
                                                      orc_BusName,
                                                      pc_UndoCommand, true);
      connect(pc_TmpAddCmd,
              &C_SdManUnoTopologyAddDeleteBaseCommand::SigErrorChange, this,
              &C_SdManUnoTopologyManager::m_OnErrorChange);
      c_Ids.clear();
      c_Ids.push_back(orc_FourUniqueIds[2]);
      //lint -e{423}  //no memory leak because of Qt memory management and redo engine handling
      pc_TmpAddCmd = new C_SdManUnoTopologyAddCommand(this->mpc_Scene, c_Ids,
                                                      C_SdManUnoTopologyAddCommand::eBUS_CONNECTOR,
                                                      orc_BusPosition,
                                                      oru64_Node1UniqueId, orc_FourUniqueIds[0],
                                                      oru8_Node1InterfaceNumber,
                                                      orc_Node1Properties,
                                                      pc_UndoCommand);
      connect(pc_TmpAddCmd,
              &C_SdManUnoTopologyAddDeleteBaseCommand::SigErrorChange, this,
              &C_SdManUnoTopologyManager::m_OnErrorChange);
      c_Ids.clear();
      c_Ids.push_back(orc_FourUniqueIds[3]);
      //lint -e{423}  //no memory leak because of Qt memory management and redo engine handling
      pc_TmpAddCmd = new C_SdManUnoTopologyAddCommand(this->mpc_Scene, c_Ids,
                                                      C_SdManUnoTopologyAddCommand::eBUS_CONNECTOR,
                                                      orc_BusPosition,
                                                      oru64_Node2UniqueId, orc_FourUniqueIds[0],
                                                      oru8_Node2InterfaceNumber,
                                                      orc_Node2Properties,
                                                      pc_UndoCommand);
      connect(pc_TmpAddCmd,
              &C_SdManUnoTopologyAddDeleteBaseCommand::SigErrorChange, this,
              &C_SdManUnoTopologyManager::m_OnErrorChange);
      this->m_DoPushAndSignalError(pc_UndoCommand);
   } //lint !e429  //no memory leak because of Qt memory management and redo engine handling
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do add one node to node connection and use an existing bus. Node 1 is new to the bus.

   \param[in]  orc_TwoUniqueIds           Two unique IDs for the two new items created by this action
   \param[in]  oru64_BusUniqueId          Bus unique ID
   \param[in]  orc_Node1Position          Node 1 position
   \param[in]  oru64_Node1UniqueId        Node 1 unique ID
   \param[in]  oru8_Node1InterfaceNumber  Node 1 interface number
   \param[in]  orc_Node1Properties        Node 1 properties
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyManager::DoAddNodeToNodeConnectionUsingExistingBus(
   const std::vector<uint64_t> & orc_TwoUniqueIds, const uint64_t & oru64_BusUniqueId,
   const QPointF & orc_Node1Position, const uint64_t & oru64_Node1UniqueId, const uint8_t & oru8_Node1InterfaceNumber,
   const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Node1Properties)
{
   if (orc_TwoUniqueIds.size() >= 2)
   {
      QUndoCommand * const pc_UndoCommand = new QUndoCommand("Add node to node connection (using existing bus)");
      vector<uint64_t> c_Ids;
      C_SdManUnoTopologyAddCommand * pc_TmpAddCmd;
      c_Ids.clear();
      c_Ids.push_back(orc_TwoUniqueIds[0]);
      //lint -e{423}  //no memory leak because of Qt memory management and redo engine handling
      pc_TmpAddCmd = new C_SdManUnoTopologyAddCommand(this->mpc_Scene, c_Ids,
                                                      C_SdManUnoTopologyAddCommand::eBUS_CONNECTOR,
                                                      orc_Node1Position, oru64_Node1UniqueId, oru64_BusUniqueId,
                                                      oru8_Node1InterfaceNumber, orc_Node1Properties,
                                                      pc_UndoCommand);
      connect(pc_TmpAddCmd,
              &C_SdManUnoTopologyAddDeleteBaseCommand::SigErrorChange, this,
              &C_SdManUnoTopologyManager::m_OnErrorChange);
      this->m_DoPushAndSignalError(pc_UndoCommand);
   } //lint !e429  //no memory leak because of Qt memory management and redo engine handling
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new data based on a snapshot and reserved IDs

   \param[in]  orc_UniqueIds         Reserved unique IDs for snapshot data
   \param[in]  orc_Snapshot            Snapshot data
   \param[in]  orc_NewPos              Position offset for all items
   \param[in]  of64_HighestUsedZetValue  Highest used Z value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyManager::DoAddSnapshot(const std::vector<uint64_t> & orc_UniqueIds,
                                              const C_SdTopologyDataSnapshot & orc_Snapshot, const QPointF & orc_NewPos,
                                              const float64_t of64_HighestUsedZetValue)
{
   C_SdManUnoTopologyAddSnapshotCommand * pc_AddCommand;

   pc_AddCommand = new C_SdManUnoTopologyAddSnapshotCommand(this->mpc_Scene, orc_Snapshot,
                                                            orc_UniqueIds, orc_NewPos, of64_HighestUsedZetValue);
   connect(pc_AddCommand,
           &C_SdManUnoTopologyAddDeleteBaseCommand::SigErrorChange, this, &C_SdManUnoTopologyManager::m_OnErrorChange);
   this->DoPush(pc_AddCommand);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reconnect to different node

   \param[in]  opc_BusConnector     Bus connector to reconnect
   \param[in]  opc_StartingNode     Initial node
   \param[in]  opc_LastNode         New node
   \param[in]  orc_ConnectionPos    Event position
   \param[in]  ors32_Interface      Interface to connect to
   \param[in]  orc_Properties       Properties
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyManager::DoReconnectNode(const C_GiLiBusConnector * const opc_BusConnector,
                                                const C_GiNode * const opc_StartingNode,
                                                const C_GiNode * const opc_LastNode, const QPointF & orc_ConnectionPos,
                                                const int32_t & ors32_Interface,
                                                const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties)
{
   if ((opc_StartingNode != NULL) && (opc_LastNode != NULL))
   {
      vector<uint64_t> c_Ids;
      uint64_t u64_Id;
      C_SdManUnoTopologyReconnectNodeCommand * pc_ReconnectCommand;

      mh_MapItemToId(opc_BusConnector, u64_Id);
      c_Ids.push_back(u64_Id);
      pc_ReconnectCommand = new C_SdManUnoTopologyReconnectNodeCommand(this->mpc_Scene, c_Ids,
                                                                       opc_StartingNode->GetId(),
                                                                       opc_LastNode->GetId(), orc_ConnectionPos,
                                                                       ors32_Interface, orc_Properties);
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

   \param[in]  opc_BusConnector     Bus connector to reconnect
   \param[in]  opc_StartingBus      Initial bus
   \param[in]  opc_LastBus          Last bus
   \param[in]  orc_ConnectionPos    Event position
   \param[in]  ors32_Interface      Interface to connect to
   \param[in]  orc_Properties       Properties
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyManager::DoReconnectBus(const C_GiLiBusConnector * const opc_BusConnector,
                                               const C_GiLiBus * const opc_StartingBus,
                                               const C_GiLiBus * const opc_LastBus, const QPointF & orc_ConnectionPos,
                                               const int32_t & ors32_Interface,
                                               const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties)
{
   if ((opc_StartingBus != NULL) && (opc_LastBus != NULL))
   {
      vector<uint64_t> c_Ids;
      uint64_t u64_Id;
      C_SdManUnoTopologyReconnectBusCommand * pc_ReconnectCommand;

      mh_MapItemToId(opc_BusConnector, u64_Id);
      c_Ids.push_back(u64_Id);
      pc_ReconnectCommand = new C_SdManUnoTopologyReconnectBusCommand(this->mpc_Scene, c_Ids,
                                                                      opc_StartingBus->GetId(),
                                                                      opc_LastBus->GetId(), orc_ConnectionPos,
                                                                      ors32_Interface, orc_Properties);
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
   \param[in]  orc_PreviousProperties        Previous properties
   \param[in]  orc_NewProperties             New properties
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyManager::DoChangeInterface(const C_GiLiBusConnector * const opc_BusConnector,
                                                  const uint8_t & oru8_PreviousInterfaceNumber,
                                                  const uint8_t & oru8_NewInterfaceNumber,
                                                  const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_PreviousProperties,
                                                  const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_NewProperties)
{
   if (opc_BusConnector != NULL)
   {
      vector<uint64_t> c_Ids;
      uint64_t u64_Id;
      C_SdManUnoTopologyChangeInterfaceCommand * pc_ReconnectCommand;

      mh_MapItemToId(opc_BusConnector, u64_Id);
      c_Ids.push_back(u64_Id);
      pc_ReconnectCommand = new C_SdManUnoTopologyChangeInterfaceCommand(this->mpc_Scene, c_Ids,
                                                                         oru8_PreviousInterfaceNumber,
                                                                         oru8_NewInterfaceNumber,
                                                                         orc_PreviousProperties, orc_NewProperties);
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
C_SebUnoSetupStyleCommand * C_SdManUnoTopologyManager::m_GetNewStyleCommand(const std::vector<uint64_t> & orc_Items,
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
   const int32_t s32_LastIndex = static_cast<int32_t>(this->count()) - 1L;
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
