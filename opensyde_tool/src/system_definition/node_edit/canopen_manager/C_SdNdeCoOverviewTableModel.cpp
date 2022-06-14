//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Table model widget with current CANopen configuration of Manager and devices on the used interfaces

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "constants.h"
#include "C_GtGetText.h"
#include "C_PuiSdUtil.h"
#include "C_SdNdeCoOverviewTableModel.h"
#include "C_PuiSdHandler.h"
#include "C_PuiSdHandlerBusLogic.h"
//#include "C_PuiSdHandlerCanOpenLogic.h"
#include "C_OSCCanOpenManagerDeviceInfo.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

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
   this->endResetModel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get node index

   \return
   Node index
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SdNdeCoOverviewTableModel::GetNodeIndex(void) const
{
   return this->mu32_NodeIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set node index

   \param[in]  ou32_NodeIndex    Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoOverviewTableModel::SetNodeIndex(const uint32 ou32_NodeIndex)
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
sint32 C_SdNdeCoOverviewTableModel::MapRowToNodeConfig(const sint32 os32_RowIndex,
                                                       C_SdNdeCoOverviewTableModel::E_NodeType & ore_NodeType,
                                                       uint8 & oru8_InterfaceNumber,
                                                       C_OSCCanInterfaceId & orc_CanInterfaceId)
{
   sint32 s32_Error = C_NO_ERR;
   bool q_DeviceFound = false;
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);

   if ((pc_Node->c_CanOpenManagers.empty() == false) &&
       (os32_RowIndex >= 0))
   {
      // we have a CANopen Manager node
      sint32 s32_CurrentDeviceIndex = 0;
      std::map<stw_types::uint8, C_OSCCanOpenManagerInfo>::const_iterator c_IterManagers;
      for (c_IterManagers = pc_Node->c_CanOpenManagers.begin();
           (c_IterManagers != pc_Node->c_CanOpenManagers.end()) && (q_DeviceFound == false);
           ++c_IterManagers)
      {
         if (s32_CurrentDeviceIndex == os32_RowIndex)
         {
            ore_NodeType = E_NodeType::eMANAGER;
            oru8_InterfaceNumber = c_IterManagers->first;
            q_DeviceFound = true;
         }
         else
         {
            s32_CurrentDeviceIndex++;
            // current device is a normal server device
            std::map<C_OSCCanInterfaceId, C_OSCCanOpenManagerDeviceInfo>::const_iterator c_IterDevices;
            for (c_IterDevices = c_IterManagers->second.c_CanOpenDevices.begin();
                 (c_IterDevices != c_IterManagers->second.c_CanOpenDevices.end()) && (q_DeviceFound == false);
                 ++c_IterDevices)
            {
               if (s32_CurrentDeviceIndex == os32_RowIndex)
               {
                  // found normal server device --> check if active otherwise take next
                  if (c_IterDevices->second.q_DeviceOptional == false)
                  {
                     ore_NodeType = E_NodeType::eDEVICE;
                     oru8_InterfaceNumber = c_IterManagers->first; // we take the interface from manager
                     orc_CanInterfaceId = c_IterDevices->first;
                     q_DeviceFound = true;
                  }
               }
               else
               {
                  s32_CurrentDeviceIndex++;
               }
            }
         }
      }
   }

   if (q_DeviceFound == false)
   {
      s32_Error = C_NOACT;
   }

   return s32_Error;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get header data

   \param[in]  osn_Section       Section
   \param[in]  oe_Orientation    Orientation
   \param[in]  osn_Role          Role

   \return
   Header string
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdNdeCoOverviewTableModel::headerData(const stw_types::sintn osn_Section,
                                                 const Qt::Orientation oe_Orientation,
                                                 const stw_types::sintn osn_Role) const
{
   QVariant c_Retval = QAbstractTableModel::headerData(osn_Section, oe_Orientation, osn_Role);

   if (oe_Orientation == Qt::Orientation::Horizontal)
   {
      const E_Columns e_Col = static_cast<E_Columns>(osn_Section);
      if (osn_Role == static_cast<sintn>(Qt::DisplayRole))
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
      else if (osn_Role == msn_USER_ROLE_TOOL_TIP_HEADING)
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
      else if (osn_Role == msn_USER_ROLE_TOOL_TIP_CONTENT)
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
      else if (osn_Role == static_cast<sintn>(Qt::TextAlignmentRole))
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
sintn C_SdNdeCoOverviewTableModel::rowCount(const QModelIndex & orc_Parent) const
{
   stw_types::sintn sn_Retval = 0;

   if (!orc_Parent.isValid())
   {
      const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);

      std::map<stw_types::uint8, C_OSCCanOpenManagerInfo>::const_iterator c_IterManagers;
      for (c_IterManagers = pc_Node->c_CanOpenManagers.begin(); c_IterManagers != pc_Node->c_CanOpenManagers.end();
           ++c_IterManagers)
      {
         sn_Retval++; // manager found

         // iterate through devices of manager
         std::map<C_OSCCanInterfaceId, C_OSCCanOpenManagerDeviceInfo>::const_iterator c_IterDevices;
         for (c_IterDevices = c_IterManagers->second.c_CanOpenDevices.begin();
              c_IterDevices != c_IterManagers->second.c_CanOpenDevices.end();
              ++c_IterDevices)
         {
            // found normal server device --> check if active otherwise take next
            if (c_IterDevices->second.q_DeviceOptional == false)
            {
               sn_Retval++;
            }
         }
      }
   }
   return sn_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get table column count

   \param[in]  orc_Parent  Parent

   \return
   Column count
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_SdNdeCoOverviewTableModel::columnCount(const QModelIndex & orc_Parent) const
{
   sintn sn_Retval = 0;

   if (!orc_Parent.isValid())
   {
      //For table parent should always be invalid
      sn_Retval = 7;
   }
   return sn_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get data at index

   \param[in]  orc_Index   Index
   \param[in]  osn_Role    Data role

   \return
   Data
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdNdeCoOverviewTableModel::data(const QModelIndex & orc_Index, const sintn osn_Role) const
{
   QVariant c_Retval;

   if (orc_Index.isValid() == true)
   {
      const E_Columns e_Col = static_cast<E_Columns>(orc_Index.column());
      if ((osn_Role == static_cast<sintn>(Qt::DisplayRole)) || (osn_Role == static_cast<sintn>(Qt::EditRole)))
      {
         sint32 s32_Error;
         C_CoTableData c_Data;
         switch (e_Col)
         {
         case eINDEX:
            c_Retval = orc_Index.row() + 1;
            break;
         case eINTERFACE:
            s32_Error = m_MapRowToCoData(orc_Index.row(), c_Data);
            if (s32_Error == C_NO_ERR)
            {
               c_Retval = c_Data.c_Interface;
            }
            break;
         case eNODE:
            s32_Error = m_MapRowToCoData(orc_Index.row(), c_Data);
            if (s32_Error == C_NO_ERR)
            {
               c_Retval = c_Data.c_Node;
            }
            break;
         case eROLE:
            s32_Error = m_MapRowToCoData(orc_Index.row(), c_Data);
            if (s32_Error == C_NO_ERR)
            {
               c_Retval = c_Data.c_Role;
            }
            break;
         case eCANOPENNODEID:
            s32_Error = m_MapRowToCoData(orc_Index.row(), c_Data);
            if (s32_Error == C_NO_ERR)
            {
               c_Retval = c_Data.c_CANopenId;
            }
            break;
         case eTPDOS:
            s32_Error = m_MapRowToCoData(orc_Index.row(), c_Data);
            if (s32_Error == C_NO_ERR)
            {
               c_Retval = c_Data.c_TPDOs;
            }
            break;
         case eRPDOS:
            s32_Error = m_MapRowToCoData(orc_Index.row(), c_Data);
            if (s32_Error == C_NO_ERR)
            {
               c_Retval = c_Data.c_RPDOs;
            }
            break;
         default:
            break;
         }
      }
      else if (osn_Role == static_cast<sintn>(Qt::TextAlignmentRole))
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
/*! \brief  Map row to data

   \param[in]  os32_RowIndex  Index
   \param[out] orc_Data       data structure to be filled

   \return
   C_NO_ACT node not available
   C_NO_ERR Valid data
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdNdeCoOverviewTableModel::m_MapRowToCoData(const sint32 os32_RowIndex, C_CoTableData & orc_Data) const
{
   sint32 s32_Error = C_NO_ERR;
   bool q_DeviceFound = false;
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
   static C_PdoCount hc_PdoManagerCnt;
   static std::map<uint32, C_PdoCount> hc_PdoDeviceMap;

   if ((pc_Node->c_CanOpenManagers.empty() == false) &&
       (os32_RowIndex >= 0))
   {
      // we have a CANopen Manager node
      uint8 u8_InterfaceIndex;
      sint32 s32_CurrentDeviceIndex = 0;
      std::map<stw_types::uint8, C_OSCCanOpenManagerInfo>::const_iterator c_IterManagers;
      for (c_IterManagers = pc_Node->c_CanOpenManagers.begin();
           (c_IterManagers != pc_Node->c_CanOpenManagers.end()) && (q_DeviceFound == false);
           ++c_IterManagers)
      {
         if (s32_CurrentDeviceIndex == os32_RowIndex)
         {
            u8_InterfaceIndex = c_IterManagers->first;
            // current device is a CANopen Manager
            orc_Data.c_Interface = C_PuiSdUtil::h_GetInterfaceName(C_OSCSystemBus::eCAN, u8_InterfaceIndex);
            //lint -e{1946} // use of functional-style cast is fine here
            orc_Data.c_Node = QString(pc_Node->c_Properties.c_Name.c_str());
            orc_Data.c_Role = "Manager";
            orc_Data.c_CANopenId = QString::number(c_IterManagers->second.u8_NodeIDValue);
            // get TPDOs and RPDOs via message container
            this->m_GetPdoCountsByManager(pc_Node, u8_InterfaceIndex, &hc_PdoManagerCnt, &hc_PdoDeviceMap);
            this->m_GetResultStringsOfPdos(&hc_PdoManagerCnt, orc_Data.c_TPDOs, orc_Data.c_RPDOs);

            q_DeviceFound = true;
         }
         else
         {
            s32_CurrentDeviceIndex++;
            // current device is a normal server device
            std::map<C_OSCCanInterfaceId, C_OSCCanOpenManagerDeviceInfo>::const_iterator c_IterDevices;
            for (c_IterDevices = c_IterManagers->second.c_CanOpenDevices.begin();
                 (c_IterDevices != c_IterManagers->second.c_CanOpenDevices.end()) && (q_DeviceFound == false);
                 ++c_IterDevices)
            {
               if (s32_CurrentDeviceIndex == os32_RowIndex)
               {
                  // found normal server device --> check if active otherwise take next
                  if (c_IterDevices->second.q_DeviceOptional == false)
                  {
                     const C_OSCNode * const pc_DeviceNode = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(
                        c_IterDevices->first.u32_NodeIndex);
                     u8_InterfaceIndex = c_IterManagers->first; // we display the CAN interface from Manager
                     orc_Data.c_Interface = C_PuiSdUtil::h_GetInterfaceName(C_OSCSystemBus::eCAN, u8_InterfaceIndex);
                     //lint -e{1946} // use of functional-style cast is fine here
                     orc_Data.c_Node = QString(pc_DeviceNode->c_Properties.c_Name.c_str());
                     orc_Data.c_Role = "Device";
                     orc_Data.c_CANopenId = QString::number(c_IterManagers->second.u8_NodeIDValue);
                     // get TPDOs and RPDOs via message container
                     const C_PdoCount * const pc_PdoCount =
                        &((hc_PdoDeviceMap.find(c_IterDevices->first.u32_NodeIndex))->second);
                     this->m_GetResultStringsOfPdos(pc_PdoCount, orc_Data.c_TPDOs, orc_Data.c_RPDOs);

                     q_DeviceFound = true;
                  }
               }
               else
               {
                  s32_CurrentDeviceIndex++;
               }
            }
         }
      }
   }

   if (q_DeviceFound == false)
   {
      s32_Error = C_NOACT;
   }

   return s32_Error;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get all PDOs of devices by manager node

   \param[in]       opc_Node             manager node
   \param[in]       ou8_InterfaceIndex   CAN interface index
   \param[out]      opc_PdoManagerCnt    PDO Rx/Tx message numbers <active/total> of manager node
   \param[out]      opc_PdoDeviceMap     PDO Rx/Tx message numbers <active/total> of device nodes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoOverviewTableModel::m_GetPdoCountsByManager(const C_OSCNode * const opc_Node,
                                                          const uint8 ou8_InterfaceIndex,
                                                          C_PdoCount * const opc_PdoManagerCnt, std::map<uint32,
                                                                                                         C_PdoCount> * const opc_PdoDeviceMap)
const
{
   opc_PdoManagerCnt->u16_RxActive = 0U;
   opc_PdoManagerCnt->u16_RxTotal = 0U;
   opc_PdoManagerCnt->u16_TxActive = 0U;
   opc_PdoManagerCnt->u16_TxTotal = 0U;
   opc_PdoDeviceMap->clear(); // empty pdo device list for current manager

   // get message container of CANopen protocol
   std::vector<C_OSCCanProtocol>::const_iterator c_IterComProtocols;
   for (c_IterComProtocols = opc_Node->c_ComProtocols.begin();
        c_IterComProtocols != opc_Node->c_ComProtocols.end();
        ++c_IterComProtocols)
   {
      if (c_IterComProtocols->e_Type == C_OSCCanProtocol::E_Type::eCAN_OPEN)
      {
         const C_OSCCanMessageContainer * const pc_MsgContainer =
            C_PuiSdHandler::h_GetInstance()->GetCanProtocolMessageContainer(
               this->mu32_NodeIndex, C_OSCCanProtocol::E_Type::eCAN_OPEN,
               ou8_InterfaceIndex, c_IterComProtocols->u32_DataPoolIndex);

         // Tx
         opc_PdoManagerCnt->u16_TxTotal = static_cast<uint16>(pc_MsgContainer->c_TxMessages.size());
         std::vector<C_OSCCanMessage>::const_iterator c_MsgIter;
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
         opc_PdoManagerCnt->u16_RxTotal = static_cast<uint16>(pc_MsgContainer->c_RxMessages.size());
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
   \param[out]      orc_ActiveTPDOs     "<active number Tx Pdos> / <total number Tx PDOs>"
   \param[out]      orc_ActiveRPDOs     "<active number Rx Pdos> / <total number Rx PDOs>"
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoOverviewTableModel::m_GetResultStringsOfPdos(
   const C_SdNdeCoOverviewTableModel::C_PdoCount * const opc_PdoCount, QString & orc_ActiveTPDOs,
   QString & orc_ActiveRPDOs) const
{
   if ((opc_PdoCount->u16_TxActive == 0U) && (opc_PdoCount->u16_TxTotal == 0U))
   {
      orc_ActiveTPDOs = "none";
   }
   else
   {
      orc_ActiveTPDOs = QString::number(opc_PdoCount->u16_TxActive) + " / " + QString::number(
         opc_PdoCount->u16_TxTotal);
   }

   if ((opc_PdoCount->u16_RxActive == 0U) && (opc_PdoCount->u16_RxTotal == 0U))
   {
      orc_ActiveRPDOs = "none";
   }
   else
   {
      orc_ActiveRPDOs = QString::number(opc_PdoCount->u16_RxActive) + " / " + QString::number(
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
void C_SdNdeCoOverviewTableModel::m_AddToMessageDeviceCount(const uint32 ou32_NodeId, const bool oq_MessageActive,
                                                            const C_SdNdeCoOverviewTableModel::E_PdoType oe_PdoType,
                                                            std::map<uint32,
                                                                     C_PdoCount> * const opc_PdoDeviceMap)
const
{
   std::map<uint32, C_PdoCount>::iterator c_MapIter;

   c_MapIter = opc_PdoDeviceMap->find(ou32_NodeId);

   if (c_MapIter != opc_PdoDeviceMap->end())
   {
      // map entry already exists for device
      if (oe_PdoType == E_PdoType::eTX)
      {
         uint16 & ru16_TmpTotal = c_MapIter->second.u16_TxTotal;
         ru16_TmpTotal++;
         if (oq_MessageActive == true)
         {
            uint16 & ru16_TmpActive = c_MapIter->second.u16_TxActive;
            ru16_TmpActive++;
         }
      }
      else
      {
         uint16 & ru16_TmpTotal = c_MapIter->second.u16_RxTotal;
         ru16_TmpTotal++;
         if (oq_MessageActive == true)
         {
            uint16 & ru16_TmpActive = c_MapIter->second.u16_RxActive;
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
