//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Table model for showing the PDO configuration of a specific device on a specific interface of
                the CANopen Manager

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"
#include <QAbstractTableModel>
#include "C_SdNdeCoPdoTableModel.h"
#include "stwtypes.h"
#include "stwerrors.h"
#include "constants.h"
#include "TGLUtils.h"
#include "C_GtGetText.h"
#include "C_PuiSdHandlerBusLogic.h"
#include "C_PuiSdHandler.h"
#include "C_OSCCanMessage.h"

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
C_SdNdeCoPdoTableModel::C_SdNdeCoPdoTableModel(QObject * const opc_Parent) :
   QAbstractTableModel(opc_Parent),
   mu32_ManagerNodeIndex(0UL),
   mu8_ManagerInterfaceId(0UL)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoPdoTableModel::UpdateData()
{
   this->beginResetModel();
   this->m_FillPdoInfo();
   this->endResetModel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get device node index

   \return
   Device node index
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SdNdeCoPdoTableModel::GetDeviceNodeIndex(void) const
{
   return this->mc_DeviceNodeInterfaceId.u32_NodeIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get node index and interface

   \param[out]  oru32_NodeIndex            Node index
   \param[out]  oru8_InterfaceId           Interface index
   \param[out]  orc_CanNodeInterfaceId     CAN node interface id
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoPdoTableModel::GetNodeIndexAndInterfaceId(uint32 & oru32_NodeIndex, uint8 & oru8_InterfaceId,
                                                        C_OSCCanInterfaceId & orc_CanNodeInterfaceId)
{
   oru32_NodeIndex = this->mu32_ManagerNodeIndex;
   oru8_InterfaceId = this->mu8_ManagerInterfaceId;
   orc_CanNodeInterfaceId = this->mc_DeviceNodeInterfaceId;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set node index and interface

   \param[in]  ou32_ManagerNodeIndex              Node index
   \param[in]  ou8_ManagerInterfaceId             Interface index
   \param[in]  orc_DeviceNodeInterfaceId          CAN node interface id
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoPdoTableModel::SetNodeIndexAndInterfaceId(const uint32 ou32_ManagerNodeIndex,
                                                        const uint8 ou8_ManagerInterfaceId,
                                                        const C_OSCCanInterfaceId & orc_DeviceNodeInterfaceId)
{
   this->beginResetModel();
   this->mu32_ManagerNodeIndex = ou32_ManagerNodeIndex;
   this->mu8_ManagerInterfaceId = ou8_ManagerInterfaceId;
   this->mc_DeviceNodeInterfaceId = orc_DeviceNodeInterfaceId;
   this->endResetModel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Map row to domain index

   \param[in]   os32_Index          Index
   \param[out]  orc_MsgId           Message id of specific row

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdNdeCoPdoTableModel::MapRowToMsgId(const sint32 os32_Index,
                                             C_OSCCanMessageIdentificationIndices & orc_MsgId) const
{
   sint32 s32_Retval = C_RANGE;

   if ((os32_Index >= 0L) &&
       (static_cast<uint32>(os32_Index) < this->mc_PdoTableData.size()))
   {
      s32_Retval = C_NO_ERR;
      orc_MsgId = this->mc_PdoTableData[os32_Index].c_MsgId;
   }

   return s32_Retval;
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
QVariant C_SdNdeCoPdoTableModel::headerData(const stw_types::sintn osn_Section, const Qt::Orientation oe_Orientation,
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
         case eENABLED:
            c_Retval = C_GtGetText::h_GetText("Enabled");
            break;
         case eNAME:
            c_Retval = C_GtGetText::h_GetText("Name");
            break;
         case eCOMMENT:
            c_Retval = C_GtGetText::h_GetText("Comment");
            break;
         case eCOTYPE:
            c_Retval = C_GtGetText::h_GetText("Type");
            break;
         case eEXTENDED:
            c_Retval = C_GtGetText::h_GetText("Extended");
            break;
         case eCOBID:
            c_Retval = C_GtGetText::h_GetText("COB-ID");
            break;
         case eDLC:
            c_Retval = C_GtGetText::h_GetText("DLC");
            break;
         case eTXMETHOD:
            c_Retval = C_GtGetText::h_GetText("Tx Method");
            break;
         case eINHIBITTIME:
            c_Retval = C_GtGetText::h_GetText("Inhibit Time [ms]");
            break;
         case eEVENTTIME:
            c_Retval = C_GtGetText::h_GetText("Event Time [ms]");
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
         case eENABLED:
            c_Retval = C_GtGetText::h_GetText("Enabled");
            break;
         case eNAME:
            c_Retval = C_GtGetText::h_GetText("Name");
            break;
         case eCOMMENT:
            c_Retval = C_GtGetText::h_GetText("Comment");
            break;
         case eCOTYPE:
            c_Retval = C_GtGetText::h_GetText("Type");
            break;
         case eEXTENDED:
            c_Retval = C_GtGetText::h_GetText("Extended");
            break;
         case eCOBID:
            c_Retval = C_GtGetText::h_GetText("COB-ID");
            break;
         case eDLC:
            c_Retval = C_GtGetText::h_GetText("DLC");
            break;
         case eTXMETHOD:
            c_Retval = C_GtGetText::h_GetText("Tx Method");
            break;
         case eINHIBITTIME:
            c_Retval = C_GtGetText::h_GetText("Inhibit Time");
            break;
         case eEVENTTIME:
            c_Retval = C_GtGetText::h_GetText("Event Time");
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
            c_Retval = C_GtGetText::h_GetText("Continuous index for all PDOs.");
            break;
         case eENABLED:
            c_Retval = C_GtGetText::h_GetText("If PDO is activated, then a 'x' sign is displayed.");
            break;
         case eNAME:
            c_Retval = C_GtGetText::h_GetText("User specific name of PDO.");
            break;
         case eCOMMENT:
            c_Retval = C_GtGetText::h_GetText("User specific comment of PDO.");
            break;
         case eCOTYPE:
            c_Retval = C_GtGetText::h_GetText("CANopen type of PDO.");
            break;
         case eEXTENDED:
            c_Retval =
               C_GtGetText::h_GetText("If Extended flag 'x' is displayed, then 29-bit CAN-IDs are used for PDO.");
            break;
         case eCOBID:
            c_Retval = C_GtGetText::h_GetText("COB-ID of PDO.");
            break;
         case eDLC:
            c_Retval = C_GtGetText::h_GetText("Data Length Code of PDO.");
            break;
         case eTXMETHOD:
            c_Retval = C_GtGetText::h_GetText(
               "Transmission type of PDO. Asynchronous manufacturer specific value of 254 " \
               "or asynchronous device specific value of 255.");
            break;
         case eINHIBITTIME:
            c_Retval = C_GtGetText::h_GetText(
               "Minimum time in milliseconds between two consecutives CAN messages of PDO.");
            break;
         case eEVENTTIME:
            c_Retval = C_GtGetText::h_GetText("Resend interval time in milliseconds of PDO.");
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
sintn C_SdNdeCoPdoTableModel::rowCount(const QModelIndex & orc_Parent) const
{
   stw_types::sintn sn_Retval = 0;

   if (!orc_Parent.isValid())
   {
      sn_Retval = this->mc_PdoTableData.size();
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
sintn C_SdNdeCoPdoTableModel::columnCount(const QModelIndex & orc_Parent) const
{
   sintn sn_Retval = 0;

   if (!orc_Parent.isValid())
   {
      //For table parent should always be invalid
      sn_Retval = 11;
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
QVariant C_SdNdeCoPdoTableModel::data(const QModelIndex & orc_Index, const sintn osn_Role) const
{
   QVariant c_Retval;

   if (orc_Index.isValid() == true)
   {
      const E_Columns e_Col = static_cast<E_Columns>(orc_Index.column());
      if ((osn_Role == static_cast<sintn>(Qt::DisplayRole)) || (osn_Role == static_cast<sintn>(Qt::EditRole)))
      {
         switch (e_Col)
         {
         case eINDEX:
            c_Retval = orc_Index.row() + 1;
            break;
         case eENABLED:
         case eEXTENDED:
            c_Retval = "";
            break;
         case eNAME:
            c_Retval = this->mc_PdoTableData.at(orc_Index.row()).c_Name;
            break;
         case eCOMMENT:
            c_Retval = this->mc_PdoTableData.at(orc_Index.row()).c_Comment;
            break;
         case eCOTYPE:
            c_Retval = this->mc_PdoTableData.at(orc_Index.row()).c_CoType;
            break;
         case eCOBID:
            c_Retval = this->mc_PdoTableData.at(orc_Index.row()).c_CobId;
            break;
         case eDLC:
            c_Retval = this->mc_PdoTableData.at(orc_Index.row()).c_Dlc;
            break;
         case eTXMETHOD:
            c_Retval = this->mc_PdoTableData.at(orc_Index.row()).c_TxMethod;
            break;
         case eINHIBITTIME:
            c_Retval = this->mc_PdoTableData.at(orc_Index.row()).c_InhibitTime;
            break;
         case eEVENTTIME:
            c_Retval = this->mc_PdoTableData.at(orc_Index.row()).c_EventTime;
            break;
         default:
            break;
         }
      }
      else if (osn_Role == static_cast<sintn>(Qt::CheckStateRole))
      {
         if (e_Col == eEXTENDED)
         {
            if (this->mc_PdoTableData.at(orc_Index.row()).q_Extended == true)
            {
               c_Retval = static_cast<sintn>(Qt::Checked);
            }
            else
            {
               c_Retval = static_cast<sintn>(Qt::Unchecked);
            }
         }
         else if (e_Col == eENABLED)
         {
            if (this->mc_PdoTableData.at(orc_Index.row()).q_Enabled == true)
            {
               c_Retval = static_cast<sintn>(Qt::Checked);
            }
            else
            {
               c_Retval = static_cast<sintn>(Qt::Unchecked);
            }
         }
         else
         {
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
/*! \brief   Get flags for item

   \param[in]  orc_Index   Item

   \return
   Flags for item
*/
//----------------------------------------------------------------------------------------------------------------------
Qt::ItemFlags C_SdNdeCoPdoTableModel::flags(const QModelIndex & orc_Index) const
{
   Qt::ItemFlags c_Retval;
   if (orc_Index.isValid() == true)
   {
      const E_Columns e_Col = static_cast<E_Columns>(orc_Index.column());
      //Each item
      c_Retval = QAbstractTableModel::flags(orc_Index);
      //Add edit
      if ((e_Col == eENABLED) || (e_Col == eEXTENDED))
      {
         //Check box
         c_Retval = c_Retval | Qt::ItemIsUserCheckable;
      }
      else
      {
         //Nothing to add
      }
   }
   else
   {
      c_Retval = Qt::NoItemFlags;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Fills a C_PdoTableData instance with all necessary data

   \param[in]  orc_Message       Original message with data to fill return value with
   \param[in]  orc_MessagegId    Message id for the original CAN message to set in the return value
   \param[in]  oe_PdoType        Information if the PDO is Tx or Rx PDO from the viewpoint of the device

   \retval   C_PdoTableData   Detailed description of 1st return value
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeCoPdoTableModel::C_PdoTableData C_SdNdeCoPdoTableModel::m_GetPdoMessageInfo(const C_OSCCanMessage & orc_Message,
                                                                                   const C_OSCCanMessageIdentificationIndices & orc_MessagegId,
                                                                                   const E_PdoType oe_PdoType)
const
{
   C_PdoTableData c_CurrentPdo;

   c_CurrentPdo.c_MsgId = orc_MessagegId;

   c_CurrentPdo.q_Enabled = orc_Message.q_CanOpenManagerMessageActive;

   //lint -e{1946} // use of functional-style cast is fine here
   c_CurrentPdo.c_Name = QString(orc_Message.c_Name.c_str());
   //lint -e{1946} // use of functional-style cast is fine here
   c_CurrentPdo.c_Comment = QString(orc_Message.c_Comment.c_str());

   if (oe_PdoType == E_PdoType::eTX)
   {
      c_CurrentPdo.c_CoType = "TPDO";
   }
   else
   {
      c_CurrentPdo.c_CoType = "RPDO";
   }

   c_CurrentPdo.q_Extended = orc_Message.q_IsExtended;

   // calculate cob id
   QString c_CobId;
   if (orc_Message.q_CanOpenManagerCobIdIncludesNodeID)
   {
      c_CobId = "0x" + QString::number(orc_Message.u32_CanId, 16).toUpper() +
                " (NodeID+0x" + QString::number(orc_Message.u32_CanOpenManagerCobIdOffset, 16).toUpper() + ")";
   }
   else
   {
      c_CobId = "0x" + QString::number(orc_Message.u32_CanId, 16).toUpper();
   }
   c_CurrentPdo.c_CobId = c_CobId;

   c_CurrentPdo.c_Dlc = QString::number(orc_Message.u16_Dlc);

   if (orc_Message.e_TxMethod == C_OSCCanMessage::E_TxMethodType::eTX_METHOD_CAN_OPEN_TYPE_254)
   {
      c_CurrentPdo.c_TxMethod = "254 - async manufacturer specific";
   }
   else
   {
      c_CurrentPdo.c_TxMethod = "255 - async device specific";
   }

   c_CurrentPdo.c_InhibitTime = QString::number(orc_Message.u16_DelayTimeMs);

   c_CurrentPdo.c_EventTime = QString::number(orc_Message.u32_CycleTimeMs);

   return c_CurrentPdo;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Collects all CANopen PDO data for PDO overview table and double click event.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoPdoTableModel::m_FillPdoInfo(void)
{
   this->mc_PdoTableData.clear();

   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(
      this->mu32_ManagerNodeIndex);

   tgl_assert(pc_Node != NULL);
   if (pc_Node != NULL)
   {
      uint32 u32_InterfaceIndex = 0U;
      bool q_IntfFound = false;
      uint32 u32_InterfaceCounter;

      // Search the interface index
      for (u32_InterfaceCounter = 0U; u32_InterfaceCounter < pc_Node->c_Properties.c_ComInterfaces.size();
           ++u32_InterfaceCounter)
      {
         if ((pc_Node->c_Properties.c_ComInterfaces[u32_InterfaceCounter].e_InterfaceType == C_OSCSystemBus::eCAN) &&
             (pc_Node->c_Properties.c_ComInterfaces[u32_InterfaceCounter].u8_InterfaceNumber ==
              this->mu8_ManagerInterfaceId))
         {
            u32_InterfaceIndex = u32_InterfaceCounter;
            q_IntfFound = true;

            break;
         }
      }

      tgl_assert(q_IntfFound == true);
      if (q_IntfFound == true)
      {
         // get message container of CANopen protocol
         std::vector<C_OSCCanProtocol>::const_iterator c_IterComProtocols;

         for (c_IterComProtocols = pc_Node->c_ComProtocols.begin();
              c_IterComProtocols != pc_Node->c_ComProtocols.end();
              ++c_IterComProtocols)
         {
            if (c_IterComProtocols->e_Type == C_OSCCanProtocol::E_Type::eCAN_OPEN)
            {
               const C_OSCCanMessageContainer * const pc_MsgContainer =
                  C_PuiSdHandler::h_GetInstance()->GetCanProtocolMessageContainer(
                     this->mu32_ManagerNodeIndex, C_OSCCanProtocol::E_Type::eCAN_OPEN,
                     u32_InterfaceIndex, c_IterComProtocols->u32_DataPoolIndex);
               C_OSCCanMessageIdentificationIndices c_CurMsgId;

               // Constant id members
               c_CurMsgId.u32_NodeIndex = this->mu32_ManagerNodeIndex;
               c_CurMsgId.e_ComProtocol = C_OSCCanProtocol::eCAN_OPEN;
               c_CurMsgId.u32_InterfaceIndex = u32_InterfaceIndex;
               c_CurMsgId.u32_DatapoolIndex = c_IterComProtocols->u32_DataPoolIndex;

               // Tx
               uint32 u32_MsgCounter;
               for (u32_MsgCounter = 0U;
                    u32_MsgCounter < pc_MsgContainer->c_TxMessages.size();
                    ++u32_MsgCounter)
               {
                  const C_OSCCanMessage & rc_Msg = pc_MsgContainer->c_TxMessages[u32_MsgCounter];
                  if (rc_Msg.c_CanOpenManagerOwnerNodeIndex == this->mc_DeviceNodeInterfaceId)
                  {
                     c_CurMsgId.u32_MessageIndex = u32_MsgCounter;
                     c_CurMsgId.q_MessageIsTx = true;

                     // add Tx message of Manager as Rx message for a Device
                     this->mc_PdoTableData.push_back(this->m_GetPdoMessageInfo(
                                                        rc_Msg,
                                                        c_CurMsgId,
                                                        E_PdoType::eRX));
                  }
               }

               // Rx
               for (u32_MsgCounter = 0U;
                    u32_MsgCounter < pc_MsgContainer->c_RxMessages.size();
                    ++u32_MsgCounter)
               {
                  const C_OSCCanMessage & rc_Msg = pc_MsgContainer->c_RxMessages[u32_MsgCounter];
                  if (rc_Msg.c_CanOpenManagerOwnerNodeIndex == this->mc_DeviceNodeInterfaceId)
                  {
                     c_CurMsgId.u32_MessageIndex = u32_MsgCounter;
                     c_CurMsgId.q_MessageIsTx = false;

                     // add Tx message of Manager as Rx message for a Device
                     this->mc_PdoTableData.push_back(this->m_GetPdoMessageInfo(
                                                        rc_Msg,
                                                        c_CurMsgId,
                                                        E_PdoType::eTX));
                  }
               }
            }
         }
      }
   }
}
