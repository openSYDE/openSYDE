//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Message table model (implementation)

   Message table model

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <iostream>

#include <QElapsedTimer>

#include "stwtypes.h"
#include "stwerrors.h"
#include "constants.h"
#include "C_SdBueMessageTableModel.h"
#include "C_GtGetText.h"
#include "C_PuiSdHandler.h"
#include "C_PuiSdUtil.h"
#include "C_SdUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
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

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueMessageTableModel::C_SdBueMessageTableModel(QObject * const opc_Parent) :
   QAbstractTableModel(opc_Parent),
   mpc_SyncManager(NULL)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set message sync manager

   \param[in,out] opc_Value Message sync manager
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageTableModel::SetMessageSyncManager(C_PuiSdNodeCanMessageSyncManager * const opc_Value)
{
   this->mpc_SyncManager = opc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Signal data changes to table
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageTableModel::UpdateData(void)
{
   if (this->mpc_SyncManager != NULL)
   {
      beginResetModel();
      this->mc_MessageIds = this->mpc_SyncManager->GetUniqueMessages();
      endResetModel();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get header data

   \param[in] osn_Section    Section
   \param[in] oe_Orientation Orientation
   \param[in] osn_Role       Role

   \return
   Header string
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdBueMessageTableModel::headerData(const sintn osn_Section, const Qt::Orientation oe_Orientation,
                                              const sintn osn_Role) const
{
   QVariant c_Retval = QAbstractTableModel::headerData(osn_Section, oe_Orientation, osn_Role);

   if (oe_Orientation == Qt::Orientation::Horizontal)
   {
      const C_SdBueMessageTableModel::E_Columns e_Col = h_ColumnToEnum(osn_Section);
      if (osn_Role == static_cast<sintn>(Qt::DisplayRole))
      {
         switch (e_Col)
         {
         case eINDEX:
            c_Retval = "#";
            break;
         case eICON:
            c_Retval = "";
            break;
         case eNAME:
            c_Retval = C_GtGetText::h_GetText("Name");
            break;
         case eCOMMENT:
            c_Retval = C_GtGetText::h_GetText("Comment");
            break;
         case eEXTENDED:
            c_Retval = C_GtGetText::h_GetText("Extended");
            break;
         case eCAN_ID:
            c_Retval = C_GtGetText::h_GetText("CAN ID");
            break;
         case eDLC:
            c_Retval = C_GtGetText::h_GetText("DLC");
            break;
         case eTX_METHOD:
            c_Retval = C_GtGetText::h_GetText("Tx method");
            break;
         case eCYCLE_TIME:
            c_Retval = C_GtGetText::h_GetText("Cycle time [ms]");
            break;
         case eNOT_EARLIER_THAN:
            c_Retval = C_GtGetText::h_GetText("Not earlier than [ms]");
            break;
         case eNOT_LATER_THAN:
            c_Retval = C_GtGetText::h_GetText("But not later than [ms]");
            break;
         case eTRANSMITTER:
            c_Retval = C_GtGetText::h_GetText("Transmitter");
            break;
         case eRECEIVER:
            c_Retval = C_GtGetText::h_GetText("Receiver");
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

   \param[in] orc_Parent Parent

   \return
   Row count
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_SdBueMessageTableModel::rowCount(const QModelIndex & orc_Parent) const
{
   stw_types::sintn sn_Retval = 0;
   if (!orc_Parent.isValid())
   {
      sn_Retval = this->mc_MessageIds.size();
   }
   return sn_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get table column count

   \param[in] orc_Parent Parent

   \return
   Column count
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_SdBueMessageTableModel::columnCount(const QModelIndex & orc_Parent) const
{
   sintn sn_Retval = 0;

   if (!orc_Parent.isValid())
   {
      //For table parent should always be invalid
      sn_Retval = 13;
   }
   return sn_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get data at index

   \param[in] orc_Index Index
   \param[in] osn_Role  Data role

   \return
   Data
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdBueMessageTableModel::data(const QModelIndex & orc_Index, const sintn osn_Role) const
{
   QVariant c_Retval;

   if (orc_Index.isValid() == true)
   {
      const C_SdBueMessageTableModel::E_Columns e_Col = h_ColumnToEnum(orc_Index.column());
      if ((osn_Role == static_cast<sintn>(Qt::DisplayRole)) || (osn_Role == static_cast<sintn>(Qt::EditRole)))
      {
         if (orc_Index.row() >= 0)
         {
            const uint32 u32_Index = static_cast<uint32>(orc_Index.row());
            if (u32_Index < this->mc_MessageIds.size())
            {
               const C_OSCCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(
                  this->mc_MessageIds[u32_Index]);
               if (pc_Message != NULL)
               {
                  switch (e_Col)
                  {
                  case eINDEX:
                     c_Retval = orc_Index.row() + 1;
                     break;
                  case eICON:
                     c_Retval = QIcon(":images/system_definition/IconMessage.svg");
                     break;
                  case eNAME:
                     c_Retval = static_cast<QString>(pc_Message->c_Name.c_str());
                     break;
                  case eCOMMENT:
                     c_Retval = static_cast<QString>(pc_Message->c_Comment.c_str());
                     break;
                  case eEXTENDED:
                     c_Retval = "";
                     break;
                  case eCAN_ID:
                     if (osn_Role == static_cast<sintn>(Qt::EditRole))
                     {
                        c_Retval = static_cast<uint64>(pc_Message->u32_CanId);
                     }
                     else
                     {
                        c_Retval = "0x" + QString::number(pc_Message->u32_CanId, 16).toUpper();
                     }
                     break;
                  case eDLC:
                     c_Retval = pc_Message->u16_Dlc;
                     break;
                  case eTX_METHOD:
                     c_Retval = C_SdUtil::h_ConvertTxMethodToName(pc_Message->e_TxMethod);
                     break;
                  case eCYCLE_TIME:
                     if (pc_Message->e_TxMethod == C_OSCCanMessage::eTX_METHOD_CYCLIC)
                     {
                        c_Retval = static_cast<uint64>(pc_Message->u32_CycleTimeMs);
                     }
                     else
                     {
                        c_Retval = "-";
                     }
                     break;
                  case eNOT_EARLIER_THAN:
                     if (pc_Message->e_TxMethod == C_OSCCanMessage::eTX_METHOD_ON_CHANGE)
                     {
                        c_Retval = pc_Message->u16_DelayTimeMs;
                     }
                     else
                     {
                        c_Retval = "-";
                     }
                     break;
                  case eNOT_LATER_THAN:
                     if (pc_Message->e_TxMethod == C_OSCCanMessage::eTX_METHOD_ON_CHANGE)
                     {
                        c_Retval = static_cast<uint64>(pc_Message->u32_CycleTimeMs);
                     }
                     else
                     {
                        c_Retval = "-";
                     }
                     break;
                  case eTRANSMITTER:
                     if (this->mpc_SyncManager != NULL)
                     {
                        const std::vector<C_OSCCanMessageIdentificationIndices> c_MatchingIds =
                           this->mpc_SyncManager->GetMatchingMessageVector(this->mc_MessageIds[u32_Index]);
                        bool q_Found = false;
                        for (uint32 u32_ItMatchingId = 0; u32_ItMatchingId < c_MatchingIds.size(); ++u32_ItMatchingId)
                        {
                           const C_OSCCanMessageIdentificationIndices & rc_CurMatchingId =
                              c_MatchingIds[u32_ItMatchingId];
                           //Transmitter
                           if (rc_CurMatchingId.q_MessageIsTx == true)
                           {
                              const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(
                                 rc_CurMatchingId.u32_NodeIndex);
                              if (pc_Node != NULL)
                              {
                                 q_Found = true;
                                 c_Retval = m_CreateNodeName(rc_CurMatchingId, c_MatchingIds);
                              }
                           }
                        }
                        if (q_Found == false)
                        {
                           c_Retval = "-";
                        }
                     }
                     break;
                  case eRECEIVER:
                     if (this->mpc_SyncManager != NULL)
                     {
                        const std::vector<C_OSCCanMessageIdentificationIndices> c_MatchingIds =
                           this->mpc_SyncManager->GetMatchingMessageVector(this->mc_MessageIds[u32_Index]);
                        QString c_Output = "";
                        bool q_Found = false;
                        for (uint32 u32_ItMatchingId = 0; u32_ItMatchingId < c_MatchingIds.size(); ++u32_ItMatchingId)
                        {
                           const C_OSCCanMessageIdentificationIndices & rc_CurMatchingId =
                              c_MatchingIds[u32_ItMatchingId];
                           //Receiver
                           if (rc_CurMatchingId.q_MessageIsTx == false)
                           {
                              if (q_Found == true)
                              {
                                 c_Output += ", ";
                              }
                              q_Found = true;
                              c_Output += this->m_CreateNodeName(rc_CurMatchingId, c_MatchingIds);
                           }
                        }
                        if (q_Found == false)
                        {
                           c_Output = "-";
                        }
                        c_Retval = c_Output;
                     }
                     break;
                  default:
                     break;
                  }
               }
            }
         }
      }
      else if (osn_Role == static_cast<sintn>(Qt::CheckStateRole))
      {
         if (orc_Index.row() >= 0)
         {
            const uint32 u32_Index = static_cast<uint32>(orc_Index.row());
            if (u32_Index < this->mc_MessageIds.size())
            {
               const C_OSCCanMessage * pc_Message;
               switch (e_Col)
               {
               case eEXTENDED:
                  pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(
                     this->mc_MessageIds[u32_Index]);
                  if (pc_Message != NULL)
                  {
                     if (pc_Message->q_IsExtended == true)
                     {
                        c_Retval = static_cast<sintn>(Qt::Checked);
                     }
                     else
                     {
                        c_Retval = static_cast<sintn>(Qt::Unchecked);
                     }
                  }
                  break;
               default:
                  break;
               }
            }
         }
      }
      else if (osn_Role == msn_USER_ROLE_ICON)
      {
         switch (e_Col)
         {
         case eICON:
            if (orc_Index.row() >= 0)
            {
               const uint32 u32_Index = static_cast<uint32>(orc_Index.row());
               if (u32_Index < this->mc_MessageIds.size())
               {
                  const C_OSCCanMessageIdentificationIndices & rc_MessageId = this->mc_MessageIds[u32_Index];
                  const C_OSCCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(
                     rc_MessageId);
                  if (pc_Message != NULL)
                  {
                     if (this->mpc_SyncManager != NULL)
                     {
                        bool q_NameValid;
                        bool q_IdValid;
                        bool q_HasTx;
                        bool q_SignalsValid = true;
                        QStringList c_Tmp;

                        this->mpc_SyncManager->CheckMessageNameBus(pc_Message->c_Name, q_NameValid, &rc_MessageId);
                        this->mpc_SyncManager->CheckMessageIdBus(pc_Message->u32_CanId, q_IdValid, &rc_MessageId);
                        this->mpc_SyncManager->CheckMessageHasTx(q_HasTx, rc_MessageId);
                        //Check signals
                        if (pc_Message->c_Signals.size() > 0)
                        {
                           const C_OSCNodeDataPoolList * const pc_List =
                              C_PuiSdHandler::h_GetInstance()->GetOSCCanDataPoolList(
                                 rc_MessageId.u32_NodeIndex,
                                 rc_MessageId.e_ComProtocol,
                                 rc_MessageId.u32_InterfaceIndex,
                                 rc_MessageId.u32_DatapoolIndex,
                                 rc_MessageId.q_MessageIsTx);
                           if (pc_List != NULL)
                           {
                              for (uint32 u32_ItSignal = 0; u32_ItSignal < pc_Message->c_Signals.size(); ++u32_ItSignal)
                              {
                                 if (pc_Message->CheckErrorSignal(pc_List, u32_ItSignal,
                                                                  C_OSCCanProtocol::h_GetCANMessageValidSignalsDLCOffset(
                                                                     rc_MessageId.e_ComProtocol)))
                                 {
                                    q_SignalsValid = false;
                                 }
                              }
                           }
                        }
                        c_Tmp.push_back(QString::number(20));
                        if ((((q_NameValid == false) || (q_IdValid == false)) || (q_SignalsValid == false)) ||
                            (q_HasTx == false))
                        {
                           c_Tmp.push_back("://images/system_definition/IconMessageError.svg");
                        }
                        else
                        {
                           c_Tmp.push_back("://images/system_definition/IconMessage.svg");
                        }
                        c_Retval = c_Tmp;
                     }
                  }
               }
            }
            break;
         case eNAME:
         case eCOMMENT:
         case eEXTENDED:
         case eCAN_ID:
         case eDLC:
         case eTX_METHOD:
         case eCYCLE_TIME:
         case eNOT_EARLIER_THAN:
         case eTRANSMITTER:
         case eRECEIVER:
         default:
            //No decoration
            break;
         }
      }
      else if (osn_Role == static_cast<sintn>(Qt::FontRole))
      {
         QFont c_Font;
         //Stylesheets do not allow access of specific columns so we need to set fonts manually
         switch (e_Col)
         {
         case eCOMMENT:
            c_Font = mc_STYLE_GUIDE_FONT_REGULAR_12;
            break;
         case eNAME:
         case eEXTENDED:
         case eCAN_ID:
         case eDLC:
         case eTX_METHOD:
         case eCYCLE_TIME:
         case eNOT_EARLIER_THAN:
         case eTRANSMITTER:
         case eRECEIVER:
         case eICON:
         default:
            c_Font = mc_STYLE_GUIDE_FONT_REGULAR_14;
            break;
         }
         //Convert point to pixel
         c_Font.setPixelSize(c_Font.pointSize());
         c_Retval = c_Font;
      }
      else if (osn_Role == static_cast<sintn>(Qt::TextAlignmentRole))
      {
         c_Retval = static_cast<QVariant>(Qt::AlignLeft | Qt::AlignVCenter);
      }
      else if (osn_Role == static_cast<sintn>(Qt::ForegroundRole))
      {
         c_Retval = mc_STYLE_GUIDE_COLOR_8;
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

   \param[in] orc_Index Item

   \return
   Flags for item
*/
//----------------------------------------------------------------------------------------------------------------------
Qt::ItemFlags C_SdBueMessageTableModel::flags(const QModelIndex & orc_Index) const
{
   Qt::ItemFlags c_Retval;
   if (orc_Index.isValid() == true)
   {
      const C_SdBueMessageTableModel::E_Columns e_Col = C_SdBueMessageTableModel::h_ColumnToEnum(orc_Index.column());
      //Each item
      c_Retval = QAbstractTableModel::flags(orc_Index);
      //Add edit
      switch (e_Col)
      {
      case eEXTENDED:
         //Check box
         c_Retval = c_Retval | Qt::ItemIsUserCheckable;
         break;
      default:
         //Nothing to add
         break;
      }
   }
   else
   {
      c_Retval = Qt::NoItemFlags;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Column to enum conversion

   \param[in]  ors32_Column Column

   \return
   Enum value
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueMessageTableModel::E_Columns C_SdBueMessageTableModel::h_ColumnToEnum(const sint32 & ors32_Column)
{
   C_SdBueMessageTableModel::E_Columns e_Retval = eNAME;
   switch (ors32_Column)
   {
   case 0:
      e_Retval = eINDEX;
      break;
   case 1:
      e_Retval = eICON;
      break;
   case 2:
      e_Retval = eNAME;
      break;
   case 3:
      e_Retval = eCOMMENT;
      break;
   case 4:
      e_Retval = eEXTENDED;
      break;
   case 5:
      e_Retval = eCAN_ID;
      break;
   case 6:
      e_Retval = eDLC;
      break;
   case 7:
      e_Retval = eTX_METHOD;
      break;
   case 8:
      e_Retval = eCYCLE_TIME;
      break;
   case 9:
      e_Retval = eNOT_EARLIER_THAN;
      break;
   case 10:
      e_Retval = eNOT_LATER_THAN;
      break;
   case 11:
      e_Retval = eTRANSMITTER;
      break;
   case 12:
      e_Retval = eRECEIVER;
      break;
   default:
      //Unknown
      break;
   }

   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Enum to column conversion

   \param[in] ore_Value Enum value

   \return
   Column
   -1 Error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdBueMessageTableModel::h_EnumToColumn(const C_SdBueMessageTableModel::E_Columns & ore_Value)
{
   sint32 s32_Retval;

   switch (ore_Value)
   {
   case eINDEX:
      s32_Retval = 0;
      break;
   case eICON:
      s32_Retval = 1;
      break;
   case eNAME:
      s32_Retval = 2;
      break;
   case eCOMMENT:
      s32_Retval = 3;
      break;
   case eEXTENDED:
      s32_Retval = 4;
      break;
   case eCAN_ID:
      s32_Retval = 5;
      break;
   case eDLC:
      s32_Retval = 6;
      break;
   case eTX_METHOD:
      s32_Retval = 7;
      break;
   case eCYCLE_TIME:
      s32_Retval = 8;
      break;
   case eNOT_EARLIER_THAN:
      s32_Retval = 9;
      break;
   case eNOT_LATER_THAN:
      s32_Retval = 10;
      break;
   case eTRANSMITTER:
      s32_Retval = 11;
      break;
   case eRECEIVER:
      s32_Retval = 12;
      break;
   default:
      s32_Retval = -1;
      break;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert row to message

   \param[in]  ors32_Row     Model row
   \param[out] orc_MessageId Message identification indices

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdBueMessageTableModel::ConvertRowToMessage(const sint32 & ors32_Row,
                                                     C_OSCCanMessageIdentificationIndices & orc_MessageId) const
{
   sint32 s32_Retval = C_NO_ERR;

   if (ors32_Row >= 0)
   {
      const uint32 u32_Row = static_cast<uint32>(ors32_Row);
      if (u32_Row < this->mc_MessageIds.size())
      {
         orc_MessageId = this->mc_MessageIds[u32_Row];
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Utility for getting Receiver/Transmitter node name for displaying

   If a node has multiple COMM datapools of same protocol type or multiple CAN interfaces connected to this bus,
   add these information to node name.

   Examples: Node / Node (CAN1) / Node (Datapool, CAN1)

   \param[in]       orc_CurMatchingId      ID of the message where the name is created for
   \param[in]       orc_AllMatchingIds     All matching Ids (needed to find out if CAN interface is ambiguous)

   \return
   Name string       Examples: MyNode / MyNode (CAN1) / MyNode (MyDatapool, CAN1)
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdBueMessageTableModel::m_CreateNodeName(const C_OSCCanMessageIdentificationIndices & orc_CurMatchingId,
                                                   const std::vector<C_OSCCanMessageIdentificationIndices> & orc_AllMatchingIds)
const
{
   QString c_Return = "";

   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(orc_CurMatchingId.u32_NodeIndex);

   if (pc_Node != NULL)
   {
      QString c_AdditionalInfo;

      c_Return += static_cast<QString>(pc_Node->c_Properties.c_Name.c_str());

      // Add additional info about Datapool and/or Interface if ambiguous
      if (pc_Node->GetCANProtocolsConst(orc_CurMatchingId.e_ComProtocol).size() > 1)
      {
         // Add datapool name if datapool of this protocol type is ambiguous
         c_AdditionalInfo +=
            pc_Node->c_DataPools.at(orc_CurMatchingId.u32_DatapoolIndex).c_Name.c_str();
      }

      // Add CAN interface name if two CAN interfaces of one node participate

      for (uint32 u32_ItOtherIds = 0; u32_ItOtherIds < orc_AllMatchingIds.size();
           ++u32_ItOtherIds) // re-iterate all corresponding message indices to check for
                             // same node and same datapool but different CAN interface
      {
         const C_OSCCanMessageIdentificationIndices & rc_OtherMatchingId =
            orc_AllMatchingIds[u32_ItOtherIds];
         if ((orc_CurMatchingId.u32_NodeIndex == rc_OtherMatchingId.u32_NodeIndex) &&
             (orc_CurMatchingId.u32_DatapoolIndex == rc_OtherMatchingId.u32_DatapoolIndex) &&
             (orc_CurMatchingId.u32_InterfaceIndex != rc_OtherMatchingId.u32_InterfaceIndex))
         {
            if (c_AdditionalInfo.isEmpty() == false)
            {
               c_AdditionalInfo += ", ";
            }
            c_AdditionalInfo +=
               C_PuiSdUtil::h_GetInterfaceName(C_OSCSystemBus::eCAN,
                                               static_cast<uint8> (orc_CurMatchingId.u32_InterfaceIndex));
            break;
         }
      }

      // finally add additional information to output
      if (c_AdditionalInfo.isEmpty() == false)
      {
         c_Return += " (" + c_AdditionalInfo + ")";
      }
   }

   return c_Return;
}
