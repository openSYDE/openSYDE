//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Table model widget with current CANopen configuration of Manager and devices on the used interfaces

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "constants.hpp"
#include "C_GtGetText.hpp"
#include "C_PuiSdUtil.hpp"
#include "C_SdNdeCoOverviewTableModel.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_PuiSdHandlerBusLogic.hpp"
//#include "C_PuiSdHandlerCanOpenLogic.hpp"
#include "C_OscCanOpenManagerDeviceInfo.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
 *
   \param[in]       opc_Parent          pointer to parent of QAbstractTableModel, default value is null pointer
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeCoOverviewTableModel::C_SdNdeCoOverviewTableModel(QObject * const opc_Parent) :
   QAbstractTableModel(opc_Parent),
   mu32_NodeIndex(0UL)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoOverviewTableModel::UpdateData()
{
   this->beginResetModel();
   this->m_FillCoInfo();
   this->endResetModel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get node index

   \return
   Node index
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SdNdeCoOverviewTableModel::GetNodeIndex(void) const
{
   return this->mu32_NodeIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set node index

   \param[in]  ou32_NodeIndex    Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoOverviewTableModel::SetNodeIndex(const uint32_t ou32_NodeIndex)
{
   this->beginResetModel();
   this->mu32_NodeIndex = ou32_NodeIndex;
   this->endResetModel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Map selected row to appropriate node.

   \param[in]       os32_RowIndex          Current selected row index of overview table
   \param[out]      ore_NodeType           Manager or Device
   \param[out]      oru8_InterfaceNumber   interface of current manager of current node by row index
   \param[out]      orc_CanInterfaceId     interface of current device node by row index (only filled if device node)

   \retval   C_NO_ERR         Node successfully found
   \retval   C_NOACT          Node not found
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdNdeCoOverviewTableModel::MapRowToNodeConfig(const int32_t os32_RowIndex,
                                                        C_SdNdeCoOverviewTableModel::E_NodeType & ore_NodeType,
                                                        uint8_t & oru8_InterfaceNumber,
                                                        C_OscCanInterfaceId & orc_CanInterfaceId)
{
   int32_t s32_Error = C_NO_ERR;

   if ((os32_RowIndex >= 0) && (static_cast<uint32_t>(os32_RowIndex) < this->mc_CoInfoAll.size()))
   {
      ore_NodeType = this->mc_CoInfoAll.at(os32_RowIndex).c_CoNodeConfig.e_NodeType;
      oru8_InterfaceNumber = this->mc_CoInfoAll.at(os32_RowIndex).c_CoNodeConfig.u8_InterfaceNumber;
      orc_CanInterfaceId = this->mc_CoInfoAll.at(os32_RowIndex).c_CoNodeConfig.c_CanInterfaceId;
   }
   else
   {
      s32_Error = C_NOACT;
   }

   return s32_Error;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get header data

   \param[in]  os32_Section       Section
   \param[in]  oe_Orientation    Orientation
   \param[in]  os32_Role          Role

   \return
   Header string
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdNdeCoOverviewTableModel::headerData(const int32_t os32_Section, const Qt::Orientation oe_Orientation,
                                                 const int32_t os32_Role) const
{
   QVariant c_Retval = QAbstractTableModel::headerData(os32_Section, oe_Orientation, os32_Role);

   if (oe_Orientation == Qt::Orientation::Horizontal)
   {
      const E_Columns e_Col = static_cast<E_Columns>(os32_Section);
      if (os32_Role == static_cast<int32_t>(Qt::DisplayRole))
      {
         switch (e_Col)
         {
         case eINDEX:
            c_Retval = "#";
            break;
         case eINTERFACE:
            c_Retval = C_GtGetText::h_GetText("Interface");
            break;
         case eNODE:
            c_Retval = C_GtGetText::h_GetText("Node");
            break;
         case eROLE:
            c_Retval = C_GtGetText::h_GetText("Role");
            break;
         case eCANOPENNODEID:
            c_Retval = C_GtGetText::h_GetText("CANopen Node ID");
            break;
         case eTPDOS:
            c_Retval = C_GtGetText::h_GetText("TPDOs");
            break;
         case eRPDOS:
            c_Retval = C_GtGetText::h_GetText("RPDOs");
            break;
         default:
            break;
         }
      }
      else if (os32_Role == ms32_USER_ROLE_TOOL_TIP_HEADING)
      {
         switch (e_Col)
         {
         case eINDEX:
            c_Retval = C_GtGetText::h_GetText("Index");
            break;
         case eINTERFACE:
            c_Retval = C_GtGetText::h_GetText("Interface");
            break;
         case eNODE:
            c_Retval = C_GtGetText::h_GetText("Node");
            break;
         case eROLE:
            c_Retval = C_GtGetText::h_GetText("Role");
            break;
         case eCANOPENNODEID:
            c_Retval = C_GtGetText::h_GetText("CANopen Node ID");
            break;
         case eTPDOS:
            c_Retval = C_GtGetText::h_GetText("TPDOs");
            break;
         case eRPDOS:
            c_Retval = C_GtGetText::h_GetText("RPDOs");
            break;
         default:
            break;
         }
      }
      else if (os32_Role == ms32_USER_ROLE_TOOL_TIP_CONTENT)
      {
         switch (e_Col)
         {
         case eINDEX:
            c_Retval = C_GtGetText::h_GetText("Continuous index for all CANopen nodes");
            break;
         case eINTERFACE:
            c_Retval = C_GtGetText::h_GetText("Used CAN interface of CANopen manager");
            break;
         case eNODE:
            c_Retval = C_GtGetText::h_GetText("Name of node in Network Topology");
            break;
         case eROLE:
            c_Retval = C_GtGetText::h_GetText(
               "Role of CANopen device in network. Two roles are specified, either manager or normal server device.");
            break;
         case eCANOPENNODEID:
            c_Retval = C_GtGetText::h_GetText("Node ID of CANopen device");
            break;
         case eTPDOS:
            c_Retval = C_GtGetText::h_GetText("Number of active to total transmit PDOs of device (active/total TPDOs)");
            break;
         case eRPDOS:
            c_Retval = C_GtGetText::h_GetText("Number of active to total receive PDOs of device (active/total RPDOs)");
            break;
         default:
            break;
         }
      }
      else if (os32_Role == static_cast<int32_t>(Qt::TextAlignmentRole))
      {
         c_Retval = static_cast<QVariant>(Qt::AlignLeft | Qt::AlignVCenter);
      }
      else
      {
         //No special handling
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get table row count

   \param[in]  orc_Parent  Parent

   \return
   Row count
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdNdeCoOverviewTableModel::rowCount(const QModelIndex & orc_Parent) const
{
   int32_t s32_Retval = 0;

   if (!orc_Parent.isValid())
   {
      const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);

      std::map<uint8_t, C_OscCanOpenManagerInfo>::const_iterator c_IterManagers;
      for (c_IterManagers = pc_Node->c_CanOpenManagers.begin(); c_IterManagers != pc_Node->c_CanOpenManagers.end();
           ++c_IterManagers)
      {
         s32_Retval++; // manager found

         // iterate through devices of manager
         std::map<C_OscCanInterfaceId, C_OscCanOpenManagerDeviceInfo>::const_iterator c_IterDevices;
         for (c_IterDevices = c_IterManagers->second.c_CanOpenDevices.begin();
              c_IterDevices != c_IterManagers->second.c_CanOpenDevices.end();
              ++c_IterDevices)
         {
            // found normal server device
            s32_Retval++;
         }
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get table column count

   \param[in]  orc_Parent  Parent

   \return
   Column count
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdNdeCoOverviewTableModel::columnCount(const QModelIndex & orc_Parent) const
{
   int32_t s32_Retval = 0;

   if (!orc_Parent.isValid())
   {
      //For table parent should always be invalid
      s32_Retval = 7;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get data at index

   \param[in]  orc_Index   Index
   \param[in]  os32_Role    Data role

   \return
   Data
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdNdeCoOverviewTableModel::data(const QModelIndex & orc_Index, const int32_t os32_Role) const
{
   QVariant c_Retval;

   if (orc_Index.isValid() == true)
   {
      const E_Columns e_Col = static_cast<E_Columns>(orc_Index.column());
      if ((os32_Role == static_cast<int32_t>(Qt::DisplayRole)) || (os32_Role == static_cast<int32_t>(Qt::EditRole)))
      {
         if ((static_cast<uint32_t>(orc_Index.row()) < this->mc_CoInfoAll.size()) && (orc_Index.row() >= 0))
         {
            switch (e_Col)
            {
            case eINDEX:
               c_Retval = orc_Index.row() + 1;
               break;
            case eINTERFACE:
               c_Retval = this->mc_CoInfoAll.at(orc_Index.row()).c_CoTableData.c_Interface;
               break;
            case eNODE:
               c_Retval = this->mc_CoInfoAll.at(orc_Index.row()).c_CoTableData.c_Node;
               break;
            case eROLE:
               c_Retval = this->mc_CoInfoAll.at(orc_Index.row()).c_CoTableData.c_Role;
               break;
            case eCANOPENNODEID:
               c_Retval = this->mc_CoInfoAll.at(orc_Index.row()).c_CoTableData.c_CanOpenId;
               break;
            case eTPDOS:
               c_Retval = this->mc_CoInfoAll.at(orc_Index.row()).c_CoTableData.c_TxPdos;
               break;
            case eRPDOS:
               c_Retval = this->mc_CoInfoAll.at(orc_Index.row()).c_CoTableData.c_RxPdos;
               break;
            default:
               break;
            }
         }
      }
      else if (os32_Role == static_cast<int32_t>(Qt::TextAlignmentRole))
      {
         c_Retval = static_cast<QVariant>(Qt::AlignLeft | Qt::AlignVCenter);
      }
      else
      {
         //Unknown
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get all PDOs of devices by manager node

   \param[in]       opc_Node             manager node
   \param[in]       ou8_InterfaceIndex   CAN interface index
   \param[out]      opc_PdoManagerCnt    PDO Rx/Tx message numbers <active/total> of manager node
   \param[out]      opc_PdoDeviceMap     PDO Rx/Tx message numbers <active/total> of device nodes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoOverviewTableModel::m_GetPdoCountsByManager(const C_OscNode * const opc_Node,
                                                          const uint8_t ou8_InterfaceIndex,
                                                          C_PdoCount * const opc_PdoManagerCnt, std::map<uint32_t,
                                                                                                         C_PdoCount> * const opc_PdoDeviceMap)
const
{
   opc_PdoManagerCnt->u16_RxActive = 0U;
   opc_PdoManagerCnt->u16_RxTotal = 0U;
   opc_PdoManagerCnt->u16_TxActive = 0U;
   opc_PdoManagerCnt->u16_TxTotal = 0U;
   opc_PdoDeviceMap->clear(); // empty pdo device list for current manager

   // get message container of CANopen protocol
   std::vector<C_OscCanProtocol>::const_iterator c_IterComProtocols;
   for (c_IterComProtocols = opc_Node->c_ComProtocols.begin();
        c_IterComProtocols != opc_Node->c_ComProtocols.end();
        ++c_IterComProtocols)
   {
      if (c_IterComProtocols->e_Type == C_OscCanProtocol::E_Type::eCAN_OPEN)
      {
         const C_OscCanMessageContainer * const pc_MsgContainer =
            C_PuiSdHandler::h_GetInstance()->GetCanProtocolMessageContainer(
               this->mu32_NodeIndex, C_OscCanProtocol::E_Type::eCAN_OPEN,
               ou8_InterfaceIndex, c_IterComProtocols->u32_DataPoolIndex);

         // Tx
         opc_PdoManagerCnt->u16_TxTotal = static_cast<uint16_t>(pc_MsgContainer->c_TxMessages.size());
         std::vector<C_OscCanMessage>::const_iterator c_MsgIter;
         for (c_MsgIter = pc_MsgContainer->c_TxMessages.begin();
              c_MsgIter != pc_MsgContainer->c_TxMessages.end();
              ++c_MsgIter)
         {
            if (c_MsgIter->q_CanOpenManagerMessageActive == true)
            {
               (opc_PdoManagerCnt->u16_TxActive)++;
            }
            // add Tx message of Manager as Rx message for a Device
            this->m_AddToMessageDeviceCount(c_MsgIter->c_CanOpenManagerOwnerNodeIndex.u32_NodeIndex,
                                            c_MsgIter->q_CanOpenManagerMessageActive, E_PdoType::eRX,
                                            opc_PdoDeviceMap);
         }

         // Rx
         opc_PdoManagerCnt->u16_RxTotal = static_cast<uint16_t>(pc_MsgContainer->c_RxMessages.size());
         for (c_MsgIter = pc_MsgContainer->c_RxMessages.begin();
              c_MsgIter != pc_MsgContainer->c_RxMessages.end();
              ++c_MsgIter)
         {
            if (c_MsgIter->q_CanOpenManagerMessageActive == true)
            {
               (opc_PdoManagerCnt->u16_RxActive)++;
            }
            // add Rx message of Manager as Tx message for a Device
            this->m_AddToMessageDeviceCount(c_MsgIter->c_CanOpenManagerOwnerNodeIndex.u32_NodeIndex,
                                            c_MsgIter->q_CanOpenManagerMessageActive, E_PdoType::eTX,
                                            opc_PdoDeviceMap);
         }

         break;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Converts C_PdoCount structure to user readable form.

   \param[in]       opc_PdoCount        PDO Rx/Tx message numbers <active/total> of current node
   \param[out]      orc_ActiveTxPdos     "<active number Tx Pdos> / <total number Tx PDOs>"
   \param[out]      orc_ActiveRxPdos     "<active number Rx Pdos> / <total number Rx PDOs>"
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoOverviewTableModel::m_GetResultStringsOfPdos(
   const C_SdNdeCoOverviewTableModel::C_PdoCount * const opc_PdoCount, QString & orc_ActiveTxPdos,
   QString & orc_ActiveRxPdos) const
{
   if ((opc_PdoCount->u16_TxActive == 0U) && (opc_PdoCount->u16_TxTotal == 0U))
   {
      orc_ActiveTxPdos = "none";
   }
   else
   {
      orc_ActiveTxPdos = QString::number(opc_PdoCount->u16_TxActive) + " / " + QString::number(
         opc_PdoCount->u16_TxTotal);
   }

   if ((opc_PdoCount->u16_RxActive == 0U) && (opc_PdoCount->u16_RxTotal == 0U))
   {
      orc_ActiveRxPdos = "none";
   }
   else
   {
      orc_ActiveRxPdos = QString::number(opc_PdoCount->u16_RxActive) + " / " + QString::number(
         opc_PdoCount->u16_RxTotal);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Helper method to add Rx/Tx message numbers to current device by manager node information.

   \param[in]       ou32_NodeId        Current node index
   \param[in]       oq_MessageActive   Flag if message is active
   \param[in]       oe_PdoType         Flag if message is Rx or Tx
   \param[in,out]   opc_PdoDeviceMap   to add a message to current PDO device map of messages
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoOverviewTableModel::m_AddToMessageDeviceCount(const uint32_t ou32_NodeId, const bool oq_MessageActive,
                                                            const C_SdNdeCoOverviewTableModel::E_PdoType oe_PdoType,
                                                            std::map<uint32_t,
                                                                     C_PdoCount> * const opc_PdoDeviceMap)
const
{
   std::map<uint32_t, C_PdoCount>::iterator c_MapIter;

   c_MapIter = opc_PdoDeviceMap->find(ou32_NodeId);

   if (c_MapIter != opc_PdoDeviceMap->end())
   {
      // map entry already exists for device
      if (oe_PdoType == E_PdoType::eTX)
      {
         uint16_t & ru16_TmpTotal = c_MapIter->second.u16_TxTotal;
         ru16_TmpTotal++;
         if (oq_MessageActive == true)
         {
            uint16_t & ru16_TmpActive = c_MapIter->second.u16_TxActive;
            ru16_TmpActive++;
         }
      }
      else
      {
         uint16_t & ru16_TmpTotal = c_MapIter->second.u16_RxTotal;
         ru16_TmpTotal++;
         if (oq_MessageActive == true)
         {
            uint16_t & ru16_TmpActive = c_MapIter->second.u16_RxActive;
            ru16_TmpActive++;
         }
      }
   }
   else
   {
      // new map entry
      C_PdoCount c_PdoCnt = {0U, 0U, 0U, 0U};
      if (oe_PdoType == E_PdoType::eTX)
      {
         c_PdoCnt.u16_TxTotal++;
         if (oq_MessageActive == true)
         {
            c_PdoCnt.u16_TxActive++;
         }
      }
      else
      {
         c_PdoCnt.u16_RxTotal++;
         if (oq_MessageActive == true)
         {
            c_PdoCnt.u16_RxActive++;
         }
      }
      opc_PdoDeviceMap->insert({ou32_NodeId, c_PdoCnt}
                               );
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Collects all CANopen data for overview table and double click event.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoOverviewTableModel::m_FillCoInfo()
{
   this->mc_CoInfoAll.clear();

   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);
   C_PdoCount c_PdoManagerCnt;
   std::map<uint32_t, C_PdoCount> c_PdoDeviceMap;

   if (pc_Node->c_CanOpenManagers.empty() == false)
   {
      // we have a CANopen Manager node
      uint8_t u8_InterfaceIndex;
      std::map<uint8_t, C_OscCanOpenManagerInfo>::const_iterator c_IterManagers;
      for (c_IterManagers = pc_Node->c_CanOpenManagers.begin();
           c_IterManagers != pc_Node->c_CanOpenManagers.end();
           ++c_IterManagers)
      {
         // current device is a CANopen Manager
         C_CoInfo c_CoInfoManager;

         c_CoInfoManager.c_CoNodeConfig.e_NodeType = E_NodeType::eMANAGER;
         u8_InterfaceIndex = c_IterManagers->first;
         c_CoInfoManager.c_CoNodeConfig.u8_InterfaceNumber = u8_InterfaceIndex;
         c_CoInfoManager.c_CoTableData.c_Interface = C_PuiSdUtil::h_GetInterfaceName(C_OscSystemBus::eCAN,
                                                                                     u8_InterfaceIndex);
         //lint -e{1946} // use of functional-style cast is fine here
         c_CoInfoManager.c_CoTableData.c_Node = QString(pc_Node->c_Properties.c_Name.c_str());
         c_CoInfoManager.c_CoTableData.c_Role = "Manager";
         c_CoInfoManager.c_CoTableData.c_CanOpenId = QString::number(c_IterManagers->second.u8_NodeIdValue);
         // get TPDOs and RPDOs via message container
         this->m_GetPdoCountsByManager(pc_Node, u8_InterfaceIndex, &c_PdoManagerCnt, &c_PdoDeviceMap);
         this->m_GetResultStringsOfPdos(&c_PdoManagerCnt, c_CoInfoManager.c_CoTableData.c_TxPdos,
                                        c_CoInfoManager.c_CoTableData.c_RxPdos);

         this->mc_CoInfoAll.push_back(c_CoInfoManager);

         // iterate throuch normal server devices
         std::map<C_OscCanInterfaceId, C_OscCanOpenManagerDeviceInfo>::const_iterator c_IterDevices;
         for (c_IterDevices = c_IterManagers->second.c_CanOpenDevices.begin();
              c_IterDevices != c_IterManagers->second.c_CanOpenDevices.end();
              ++c_IterDevices)
         {
            // found normal server device
            C_CoInfo c_CoInfoDevice;

            const C_OscNode * const pc_DeviceNode = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(
               c_IterDevices->first.u32_NodeIndex);
            c_CoInfoDevice.c_CoNodeConfig.e_NodeType = E_NodeType::eDEVICE;
            u8_InterfaceIndex = c_IterManagers->first; // we display the CAN interface from Manager
            c_CoInfoDevice.c_CoNodeConfig.u8_InterfaceNumber = u8_InterfaceIndex;
            c_CoInfoDevice.c_CoTableData.c_Interface = C_PuiSdUtil::h_GetInterfaceName(C_OscSystemBus::eCAN,
                                                                                       u8_InterfaceIndex);
            c_CoInfoDevice.c_CoNodeConfig.c_CanInterfaceId = c_IterDevices->first;
            //lint -e{1946} // use of functional-style cast is fine here
            c_CoInfoDevice.c_CoTableData.c_Node = QString(pc_DeviceNode->c_Properties.c_Name.c_str());
            c_CoInfoDevice.c_CoTableData.c_Role = "Device";
            c_CoInfoDevice.c_CoTableData.c_CanOpenId = QString::number(c_IterDevices->second.u8_NodeIdValue);
            // get TPDOs and RPDOs via message container
            C_PdoCount c_PdoCount = {0U, 0U, 0U, 0U};
            if (c_PdoDeviceMap.find(c_IterDevices->first.u32_NodeIndex) != c_PdoDeviceMap.end())
            {
               c_PdoCount = (c_PdoDeviceMap.find(c_IterDevices->first.u32_NodeIndex))->second;
            }
            this->m_GetResultStringsOfPdos(&c_PdoCount, c_CoInfoDevice.c_CoTableData.c_TxPdos,
                                           c_CoInfoDevice.c_CoTableData.c_RxPdos);

            this->mc_CoInfoAll.push_back(c_CoInfoDevice);
         }
      }
   }
}
