//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Message table model (implementation)

   Message table model

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <iostream>

#include <QElapsedTimer>

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "constants.hpp"
#include "C_SdBueMessageTableModel.hpp"
#include "C_GtGetText.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_PuiSdUtil.hpp"
#include "C_SdUtil.hpp"
#include "C_OscCanUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
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

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueMessageTableModel::C_SdBueMessageTableModel(QObject * const opc_Parent) :
   QAbstractTableModel(opc_Parent),
   mpc_SyncManager(NULL)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set message sync manager

   \param[in,out]  opc_Value  Message sync manager
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
      this->m_FillMsgInfo();
      endResetModel();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get header data

   \param[in]  os32_Section      Section
   \param[in]  oe_Orientation    Orientation
   \param[in]  os32_Role         Role

   \return
   Header string
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdBueMessageTableModel::headerData(const int32_t os32_Section, const Qt::Orientation oe_Orientation,
                                              const int32_t os32_Role) const
{
   QVariant c_Retval = QAbstractTableModel::headerData(os32_Section, oe_Orientation, os32_Role);

   if (oe_Orientation == Qt::Orientation::Horizontal)
   {
      const C_SdBueMessageTableModel::E_Columns e_Col = h_ColumnToEnum(os32_Section);
      if (os32_Role == static_cast<int32_t>(Qt::DisplayRole))
      {
         switch (e_Col)
         {
         case eINDEX:
            c_Retval = "#";
            break;
         case eICON:
            c_Retval = "";
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
         case eJ1939_PGN:
            c_Retval = C_GtGetText::h_GetText("PGN");
            break;
         case eJ1939_PRIORITY:
            c_Retval = C_GtGetText::h_GetText("Priority");
            break;
         case eJ1939_SOURCE_ADDRESS:
            c_Retval = C_GtGetText::h_GetText("Source Address");
            break;
         case eJ1939_DESTINATION_ADDRESS:
            c_Retval = C_GtGetText::h_GetText("Destination Address");
            break;
         case eJ1939_FORMAT:
            c_Retval = C_GtGetText::h_GetText("Format");
            break;
         case eJ1939_EDP_AND_DP:
            c_Retval = C_GtGetText::h_GetText("EDP/DP");
            break;
         case eCAN_OPEN_INDEX:
            c_Retval = C_GtGetText::h_GetText("Index");
            break;
         case eEXTENDED:
            c_Retval = C_GtGetText::h_GetText("Extended");
            break;
         case eCAN_ID:
            c_Retval = C_GtGetText::h_GetText("CAN ID");
            break;
         case eCOB_ID:
            c_Retval = C_GtGetText::h_GetText("COB-ID");
            break;
         case eDLC:
            c_Retval = C_GtGetText::h_GetText("DLC");
            break;
         case eTX_METHOD:
            c_Retval = C_GtGetText::h_GetText("Tx method");
            break;
         case eCYCLE_TIME:
            c_Retval = this->m_GetCycleTimeHeaderName();
            break;
         case eNOT_EARLIER_THAN:
            c_Retval = this->m_GetNotEarlierThanHeaderName();
            break;
         case eNOT_LATER_THAN:
            c_Retval = this->m_GetNotLaterThanHeaderName();
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
int32_t C_SdBueMessageTableModel::rowCount(const QModelIndex & orc_Parent) const
{
   int32_t s32_Retval = 0;

   if (!orc_Parent.isValid())
   {
      s32_Retval = this->mc_MsgInfoAll.size();
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
int32_t C_SdBueMessageTableModel::columnCount(const QModelIndex & orc_Parent) const
{
   int32_t s32_Retval = 0;

   if (!orc_Parent.isValid())
   {
      //For table parent should always be invalid
      s32_Retval = 22;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get data at index

   \param[in]  orc_Index   Index
   \param[in]  os32_Role   Data role

   \return
   Data
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdBueMessageTableModel::data(const QModelIndex & orc_Index, const int32_t os32_Role) const
{
   QVariant c_Retval;

   if (orc_Index.isValid() == true)
   {
      const C_SdBueMessageTableModel::E_Columns e_Col = h_ColumnToEnum(orc_Index.column());
      if ((os32_Role == static_cast<int32_t>(Qt::DisplayRole)) || (os32_Role == static_cast<int32_t>(Qt::EditRole)))
      {
         const int32_t s32_Index = orc_Index.row();
         if (static_cast<uint32_t>(s32_Index) < this->mc_MsgInfoAll.size())
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
               c_Retval = this->mc_MsgInfoAll[s32_Index].c_MessageData.c_Name;
               break;
            case eCOMMENT:
               c_Retval = this->mc_MsgInfoAll[s32_Index].c_MessageData.c_Comment;
               break;
            case eJ1939_PGN:
               c_Retval = this->mc_MsgInfoAll[s32_Index].c_MessageData.c_J1939Pgn;
               break;
            case eJ1939_PRIORITY:
               c_Retval = this->mc_MsgInfoAll[s32_Index].c_MessageData.c_J1939Priority;
               break;
            case eJ1939_SOURCE_ADDRESS:
               c_Retval = this->mc_MsgInfoAll[s32_Index].c_MessageData.c_J1939SourceAddress;
               break;
            case eJ1939_DESTINATION_ADDRESS:
               c_Retval = this->mc_MsgInfoAll[s32_Index].c_MessageData.c_J1939DestinationAddress;
               break;
            case eJ1939_FORMAT:
               c_Retval = this->mc_MsgInfoAll[s32_Index].c_MessageData.c_J1939Format;
               break;
            case eJ1939_EDP_AND_DP:
               c_Retval = this->mc_MsgInfoAll[s32_Index].c_MessageData.c_J1939EdpAndDp;
               break;
            case eCAN_OPEN_INDEX:
               if (os32_Role == static_cast<int32_t>(Qt::EditRole))
               {
                  c_Retval = this->mc_MsgInfoAll[s32_Index].c_MessageData.c_CoIndexEditRole;
               }
               else
               {
                  c_Retval = this->mc_MsgInfoAll[s32_Index].c_MessageData.c_CoIndex;
               }
               break;
            case eENABLED:
            case eEXTENDED:
               c_Retval = "";
               break;
            case eCOB_ID:
               if (os32_Role == static_cast<int32_t>(Qt::EditRole))
               {
                  c_Retval = this->mc_MsgInfoAll[s32_Index].c_MessageData.c_CobIdEditRole;
               }
               else
               {
                  c_Retval = this->mc_MsgInfoAll[s32_Index].c_MessageData.c_CobId;
               }
               break;
            case eCAN_ID:
               if (os32_Role == static_cast<int32_t>(Qt::EditRole))
               {
                  c_Retval = this->mc_MsgInfoAll[s32_Index].c_MessageData.c_CanIdEditRole;
               }
               else
               {
                  c_Retval = this->mc_MsgInfoAll[s32_Index].c_MessageData.c_CanId;
               }
               break;
            case eDLC:
               c_Retval = this->mc_MsgInfoAll[s32_Index].c_MessageData.u16_Dlc;
               break;
            case eTX_METHOD:
               c_Retval = this->mc_MsgInfoAll[s32_Index].c_MessageData.c_TxMethod;
               break;
            case eCYCLE_TIME:
               c_Retval = this->mc_MsgInfoAll[s32_Index].c_MessageData.c_CycleTime;

               break;
            case eNOT_EARLIER_THAN:
               c_Retval = this->mc_MsgInfoAll[s32_Index].c_MessageData.c_NotEarlierThan;
               break;
            case eNOT_LATER_THAN:
               c_Retval = this->mc_MsgInfoAll[s32_Index].c_MessageData.c_NotLaterThan;
               break;
            case eTRANSMITTER:
               c_Retval = this->mc_MsgInfoAll[s32_Index].c_MessageData.c_Transmitter;
               break;
            case eRECEIVER:
               c_Retval = this->mc_MsgInfoAll[s32_Index].c_MessageData.c_Receiver;
               break;
            default:
               break;
            }
         }
      }
      else if (os32_Role == static_cast<int32_t>(Qt::CheckStateRole))
      {
         const int32_t s32_Index = orc_Index.row();
         if (static_cast<uint32_t>(s32_Index) < this->mc_MsgInfoAll.size())
         {
            if (e_Col == eEXTENDED)
            {
               c_Retval = this->mc_MsgInfoAll[s32_Index].c_MessageData.s32_Extended;
            }
            else if (e_Col == eENABLED)
            {
               c_Retval = this->mc_MsgInfoAll[s32_Index].c_MessageData.s32_Enabled;
            }
            else
            {
            }
         }
      }
      else if (os32_Role == ms32_USER_ROLE_ICON)
      {
         switch (e_Col)
         {
         case eICON:
            {
               const int32_t s32_Index = orc_Index.row();
               if (static_cast<uint32_t>(s32_Index) < this->mc_MsgInfoAll.size())
               {
                  c_Retval = this->mc_MsgInfoAll[s32_Index].c_MessageData.c_Icon;
               }
               break;
            }
         case eINDEX:
         case eNAME:
         case eCOMMENT:
         case eEXTENDED:
         case eCAN_ID:
         case eDLC:
         case eTX_METHOD:
         case eCYCLE_TIME:
         case eNOT_EARLIER_THAN:
         case eNOT_LATER_THAN:
         case eTRANSMITTER:
         case eRECEIVER:
         case eENABLED:
         case eCOB_ID:
         case eJ1939_PGN:
         case eJ1939_PRIORITY:
         case eJ1939_SOURCE_ADDRESS:
         case eJ1939_DESTINATION_ADDRESS:
         case eJ1939_FORMAT:
         case eJ1939_EDP_AND_DP:
         case eCAN_OPEN_INDEX:
         default:
            //No decoration
            break;
         }
      }
      else if (os32_Role == static_cast<int32_t>(Qt::FontRole))
      {
         QFont c_Font;
         //Stylesheets do not allow access of specific columns so we need to set fonts manually
         switch (e_Col)
         {
         case eCOMMENT:
            c_Font = mc_STYLE_GUIDE_FONT_REGULAR_12;
            break;
         case eINDEX:
         case eICON:
         case eNAME:
         case eEXTENDED:
         case eCAN_ID:
         case eDLC:
         case eTX_METHOD:
         case eCYCLE_TIME:
         case eNOT_EARLIER_THAN:
         case eNOT_LATER_THAN:
         case eTRANSMITTER:
         case eRECEIVER:
         case eENABLED:
         case eCOB_ID:
         case eJ1939_PGN:
         case eJ1939_PRIORITY:
         case eJ1939_SOURCE_ADDRESS:
         case eJ1939_DESTINATION_ADDRESS:
         case eJ1939_FORMAT:
         case eJ1939_EDP_AND_DP:
         case eCAN_OPEN_INDEX:
         default:
            c_Font = mc_STYLE_GUIDE_FONT_REGULAR_14;
            break;
         }
         //Convert point to pixel
         c_Font.setPixelSize(c_Font.pointSize());
         c_Retval = c_Font;
      }
      else if (os32_Role == static_cast<int32_t>(Qt::TextAlignmentRole))
      {
         c_Retval = static_cast<QVariant>(Qt::AlignLeft | Qt::AlignVCenter);
      }
      else if (os32_Role == static_cast<int32_t>(Qt::ForegroundRole))
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

   \param[in]  orc_Index   Item

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
/*! \brief   Column to enum conversion

   \param[in]  ors32_Column   Column

   \return
   Enum value
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueMessageTableModel::E_Columns C_SdBueMessageTableModel::h_ColumnToEnum(const int32_t & ors32_Column)
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
      e_Retval = eENABLED;
      break;
   case 3:
      e_Retval = eNAME;
      break;
   case 4:
      e_Retval = eCOMMENT;
      break;
   case 5:
      e_Retval = eCAN_OPEN_INDEX;
      break;
   case 6:
      e_Retval = eEXTENDED;
      break;
   case 7:
      e_Retval = eCAN_ID;
      break;
   case 8:
      e_Retval = eJ1939_PGN;
      break;
   case 9:
      e_Retval = eJ1939_PRIORITY;
      break;
   case 10:
      e_Retval = eJ1939_SOURCE_ADDRESS;
      break;
   case 11:
      e_Retval = eJ1939_DESTINATION_ADDRESS;
      break;
   case 12:
      e_Retval = eJ1939_FORMAT;
      break;
   case 13:
      e_Retval = eJ1939_EDP_AND_DP;
      break;
   case 14:
      e_Retval = eCOB_ID;
      break;
   case 15:
      e_Retval = eDLC;
      break;
   case 16:
      e_Retval = eTX_METHOD;
      break;
   case 17:
      e_Retval = eCYCLE_TIME;
      break;
   case 18:
      e_Retval = eNOT_EARLIER_THAN;
      break;
   case 19:
      e_Retval = eNOT_LATER_THAN;
      break;
   case 20:
      e_Retval = eTRANSMITTER;
      break;
   case 21:
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

   \param[in]  ore_Value   Enum value

   \return
   Column
   -1 Error
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdBueMessageTableModel::h_EnumToColumn(const C_SdBueMessageTableModel::E_Columns & ore_Value)
{
   int32_t s32_Retval;

   switch (ore_Value)
   {
   case eINDEX:
      s32_Retval = 0;
      break;
   case eICON:
      s32_Retval = 1;
      break;
   case eENABLED:
      s32_Retval = 2;
      break;
   case eNAME:
      s32_Retval = 3;
      break;
   case eCOMMENT:
      s32_Retval = 4;
      break;
   case eCAN_OPEN_INDEX:
      s32_Retval = 5;
      break;
   case eEXTENDED:
      s32_Retval = 6;
      break;
   case eCAN_ID:
      s32_Retval = 7;
      break;
   case eJ1939_PGN:
      s32_Retval = 8;
      break;
   case eJ1939_PRIORITY:
      s32_Retval = 9;
      break;
   case eJ1939_SOURCE_ADDRESS:
      s32_Retval = 10;
      break;
   case eJ1939_DESTINATION_ADDRESS:
      s32_Retval = 11;
      break;
   case eJ1939_FORMAT:
      s32_Retval = 12;
      break;
   case eJ1939_EDP_AND_DP:
      s32_Retval = 13;
      break;
   case eCOB_ID:
      s32_Retval = 14;
      break;
   case eDLC:
      s32_Retval = 15;
      break;
   case eTX_METHOD:
      s32_Retval = 16;
      break;
   case eCYCLE_TIME:
      s32_Retval = 17;
      break;
   case eNOT_EARLIER_THAN:
      s32_Retval = 18;
      break;
   case eNOT_LATER_THAN:
      s32_Retval = 19;
      break;
   case eTRANSMITTER:
      s32_Retval = 20;
      break;
   case eRECEIVER:
      s32_Retval = 21;
      break;
   default:
      s32_Retval = -1;
      break;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert row to message

   \param[in]   ors32_Row        Model row
   \param[out]  orc_MessageId    Message identification indices

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdBueMessageTableModel::ConvertRowToMessage(const int32_t & ors32_Row,
                                                      C_OscCanMessageIdentificationIndices & orc_MessageId) const
{
   int32_t s32_Retval = C_NO_ERR;

   if (ors32_Row >= 0)
   {
      const uint32_t u32_Row = static_cast<uint32_t>(ors32_Row);
      if (u32_Row < this->mc_MsgInfoAll.size())
      {
         orc_MessageId = this->mc_MsgInfoAll[u32_Row].c_MessageId;
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

   \param[in]  orc_CurMatchingId    ID of the message where the name is created for
   \param[in]  orc_AllMatchingIds   All matching Ids (needed to find out if CAN interface is ambiguous)

   \return
   Name string       Examples: MyNode / MyNode (CAN1) / MyNode (MyDatapool, CAN1)
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdBueMessageTableModel::m_CreateNodeName(const C_OscCanMessageIdentificationIndices & orc_CurMatchingId,
                                                   const std::vector<C_OscCanMessageIdentificationIndices> & orc_AllMatchingIds)
const
{
   QString c_Return = "";

   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(orc_CurMatchingId.u32_NodeIndex);

   if (pc_Node != NULL)
   {
      QString c_AdditionalInfo;

      c_Return += static_cast<QString>(pc_Node->c_Properties.c_Name.c_str());

      // Add additional info about Datapool and/or Interface if ambiguous
      if (pc_Node->GetCanProtocolsConst(orc_CurMatchingId.e_ComProtocol).size() > 1)
      {
         // Add datapool name if datapool of this protocol type is ambiguous
         c_AdditionalInfo +=
            pc_Node->c_DataPools.at(orc_CurMatchingId.u32_DatapoolIndex).c_Name.c_str();
      }

      // Add CAN interface name if two CAN interfaces of one node participate

      for (uint32_t u32_ItOtherIds = 0; u32_ItOtherIds < orc_AllMatchingIds.size();
           ++u32_ItOtherIds) // re-iterate all corresponding message indices to check for
                             // same node and same datapool but different CAN interface
      {
         const C_OscCanMessageIdentificationIndices & rc_OtherMatchingId =
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
               C_PuiSdUtil::h_GetInterfaceName(C_OscSystemBus::eCAN,
                                               static_cast<uint8_t> (orc_CurMatchingId.u32_InterfaceIndex));
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get not earlier than header name

   \return
   Not earlier than header name
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdBueMessageTableModel::m_GetNotEarlierThanHeaderName() const
{
   QString c_Retval = C_GtGetText::h_GetText("Not earlier than [ms]");

   if (this->mpc_SyncManager != NULL)
   {
      if (this->mpc_SyncManager->GetCurrentComProtocol() == C_OscCanProtocol::eCAN_OPEN)
      {
         c_Retval = C_GtGetText::h_GetText("Inhibit Time [ms]");
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get not later than header name

   \return
   Not later than header name
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdBueMessageTableModel::m_GetNotLaterThanHeaderName() const
{
   QString c_Retval = C_GtGetText::h_GetText("But not later than [ms]");

   if (this->mpc_SyncManager != NULL)
   {
      if (this->mpc_SyncManager->GetCurrentComProtocol() == C_OscCanProtocol::eCAN_OPEN)
      {
         c_Retval = C_GtGetText::h_GetText("Event Time [ms]");
      }
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get CANopen index

   \param[in]  orc_Id         Id
   \param[in]  orc_Message    Message
   \param[in]  os32_Role       Role

   \return
   CANopen index
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdBueMessageTableModel::m_GetCanOpenIndex(const C_OscCanMessageIdentificationIndices & orc_Id,
                                                     const C_OscCanMessage & orc_Message, const int32_t os32_Role) const
{
   const uint32_t u32_ObjectIndex =
      C_OscCanOpenObjectDictionary::h_GetCanOpenObjectDictionaryIndexForPdo(
         orc_Message.u16_CanOpenManagerPdoIndex, !orc_Id.q_MessageIsTx);
   QVariant c_Retval;

   if (os32_Role == static_cast<int32_t>(Qt::EditRole))
   {
      c_Retval = static_cast<int32_t>(u32_ObjectIndex);
   }
   else
   {
      c_Retval = QString::number(u32_ObjectIndex, 16).toUpper();
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get COB-ID

   \param[in]  orc_Message    Message
   \param[in]  os32_Role       Role

   \return
   COB-ID
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdBueMessageTableModel::m_GetCobId(const C_OscCanMessage & orc_Message, const int32_t os32_Role) const
{
   QVariant c_Retval;

   if (os32_Role == static_cast<int32_t>(Qt::EditRole))
   {
      c_Retval = static_cast<int64_t>(orc_Message.u32_CanId);
   }
   else
   {
      const QString c_CanId = "0x" + QString::number(orc_Message.u32_CanId, 16).toUpper();
      if (orc_Message.q_CanOpenManagerCobIdIncludesNodeId)
      {
         const QString c_CobId = "0x" + QString::number(orc_Message.u32_CanOpenManagerCobIdOffset, 16).toUpper();
         c_Retval = static_cast<QString>("%1 ($CANopenNodeID + %2)").arg(c_CanId, c_CobId);
      }
      else
      {
         c_Retval = c_CanId;
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get not earlier than value

   \param[in]  orc_Message    Message

   \return
   Not earlier than value
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdBueMessageTableModel::m_GetNotEarlierThanValue(const C_OscCanMessage & orc_Message) const
{
   QVariant c_Retval;

   if ((orc_Message.e_TxMethod == C_OscCanMessage::eTX_METHOD_ON_CHANGE) ||
       ((this->mpc_SyncManager != NULL) &&
        (this->mpc_SyncManager->GetCurrentComProtocol() == C_OscCanProtocol::eCAN_OPEN)))
   {
      c_Retval = orc_Message.u16_DelayTimeMs;
   }
   else
   {
      c_Retval = "-";
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get not later than value

   \param[in]  orc_Message    Message

   \return
   Not later than value
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdBueMessageTableModel::m_GetNotLaterThanValue(const C_OscCanMessage & orc_Message) const
{
   QVariant c_Retval;

   if ((orc_Message.e_TxMethod == C_OscCanMessage::eTX_METHOD_ON_CHANGE) ||
       ((this->mpc_SyncManager != NULL) &&
        (this->mpc_SyncManager->GetCurrentComProtocol() == C_OscCanProtocol::eCAN_OPEN)))
   {
      c_Retval = static_cast<uint64_t>(orc_Message.u32_CycleTimeMs);
   }
   else
   {
      c_Retval = "-";
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get transmitter

   \param[in]  orc_Id         Id
   \param[in]  orc_Message    Message

   \return
   Transmitter
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdBueMessageTableModel::m_GetTransmitter(const C_OscCanMessageIdentificationIndices & orc_Id,
                                                   const C_OscCanMessage & orc_Message) const
{
   QString c_Retval;

   if (this->mpc_SyncManager != NULL)
   {
      const std::vector<C_OscCanMessageIdentificationIndices> c_MatchingIds =
         this->mpc_SyncManager->GetMatchingMessageVector(orc_Id);
      bool q_Found = false;
      for (uint32_t u32_ItMatchingId = 0; u32_ItMatchingId < c_MatchingIds.size(); ++u32_ItMatchingId)
      {
         const C_OscCanMessageIdentificationIndices & rc_CurMatchingId =
            c_MatchingIds[u32_ItMatchingId];
         //Transmitter
         if (rc_CurMatchingId.q_MessageIsTx == true)
         {
            const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(
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
         if (this->mpc_SyncManager->GetCurrentComProtocol() == C_OscCanProtocol::eCAN_OPEN)
         {
            c_Retval = this->m_GetCanOpenNode(orc_Message);
         }
         else
         {
            c_Retval = "-";
         }
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get receiver

   \param[in]  orc_Id         Id
   \param[in]  orc_Message    Message

   \return
   Receiver
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdBueMessageTableModel::m_GetReceiver(const C_OscCanMessageIdentificationIndices & orc_Id,
                                                const C_OscCanMessage & orc_Message) const
{
   QString c_Retval;

   if (this->mpc_SyncManager != NULL)
   {
      const std::vector<C_OscCanMessageIdentificationIndices> c_MatchingIds =
         this->mpc_SyncManager->GetMatchingMessageVector(orc_Id);
      QString c_Output = "";
      bool q_Found = false;
      for (uint32_t u32_ItMatchingId = 0; u32_ItMatchingId < c_MatchingIds.size(); ++u32_ItMatchingId)
      {
         const C_OscCanMessageIdentificationIndices & rc_CurMatchingId =
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
         if (this->mpc_SyncManager->GetCurrentComProtocol() == C_OscCanProtocol::eCAN_OPEN)
         {
            c_Output = this->m_GetCanOpenNode(orc_Message);
         }
         else
         {
            c_Output = "-";
         }
      }
      c_Retval = c_Output;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get CANopen node

   \param[in]  orc_Message    Message

   \return
   CANopen node
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdBueMessageTableModel::m_GetCanOpenNode(const C_OscCanMessage & orc_Message) const
{
   QString c_Retval = "-";
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(
      orc_Message.c_CanOpenManagerOwnerNodeIndex.u32_NodeIndex);

   if (pc_Node != NULL)
   {
      c_Retval = pc_Node->c_Properties.c_Name.c_str();
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get message icon

   \param[in]  orc_MessageId  Message id

   \return
   Message icon
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdBueMessageTableModel::m_GetMessageIcon(const C_OscCanMessageIdentificationIndices & orc_MessageId) const
{
   QVariant c_Retval;
   const C_OscCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(
      orc_MessageId);

   if (pc_Message != NULL)
   {
      if (this->mpc_SyncManager != NULL)
      {
         bool q_NameValid;
         bool q_IdValid;
         bool q_HasTx;
         bool q_SignalsValid = true;
         QStringList c_Tmp;

         this->mpc_SyncManager->CheckMessageNameBus(pc_Message->c_Name, q_NameValid, &orc_MessageId);
         this->mpc_SyncManager->CheckMessageIdBus(C_OscCanMessageUniqueId(pc_Message->u32_CanId,
                                                                          pc_Message->q_IsExtended), q_IdValid,
                                                  &orc_MessageId);
         this->mpc_SyncManager->CheckMessageHasTx(q_HasTx, orc_MessageId);
         //Check signals
         if (pc_Message->c_Signals.size() > 0)
         {
            const C_OscNodeDataPoolList * const pc_List =
               C_PuiSdHandler::h_GetInstance()->GetOscCanDataPoolList(
                  orc_MessageId.u32_NodeIndex,
                  orc_MessageId.e_ComProtocol,
                  orc_MessageId.u32_InterfaceIndex,
                  orc_MessageId.u32_DatapoolIndex,
                  orc_MessageId.q_MessageIsTx);
            if (pc_List != NULL)
            {
               for (uint32_t u32_ItSignal = 0; u32_ItSignal < pc_Message->c_Signals.size(); ++u32_ItSignal)
               {
                  if (pc_Message->CheckErrorSignal(
                         pc_List, u32_ItSignal,
                         C_OscCanProtocol::h_GetCanMessageValidSignalsDlcOffset(
                            orc_MessageId.e_ComProtocol),
                         C_OscCanProtocol::h_GetCanMessageSignalGapsValid(
                            orc_MessageId.e_ComProtocol),
                         C_OscCanProtocol::h_GetCanMessageSignalByteAlignmentRequired(
                            orc_MessageId.e_ComProtocol)))
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
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Collects all message data for overview table
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageTableModel::m_FillMsgInfo(void)
{
   std::vector<C_OscCanMessageIdentificationIndices> c_MessageIds =
      this->mpc_SyncManager->GetUniqueMessages();
   uint32_t u32_Counter;

   this->mc_MsgInfoAll.clear();
   this->mc_MsgInfoAll.resize(c_MessageIds.size());

   for (u32_Counter = 0U; u32_Counter < c_MessageIds.size(); ++u32_Counter)
   {
      const C_OscCanMessageIdentificationIndices & rc_Id = c_MessageIds[u32_Counter];

      // Copy the original message id
      this->mc_MsgInfoAll[u32_Counter].c_MessageId = rc_Id;

      const C_OscCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(rc_Id);
      if (pc_Message != NULL)
      {
         C_MsgTableData & rc_Data = this->mc_MsgInfoAll[u32_Counter].c_MessageData;
         // Get the data for the message
         rc_Data.c_Name = static_cast<QString>(pc_Message->c_Name.c_str());
         rc_Data.c_Comment = static_cast<QString>(pc_Message->c_Comment.c_str());
         rc_Data.c_CoIndex = m_GetCanOpenIndex(rc_Id, *pc_Message, 0);
         rc_Data.c_CoIndexEditRole = m_GetCanOpenIndex(rc_Id, *pc_Message, static_cast<int32_t>(Qt::EditRole));

         if (pc_Message->q_CanOpenManagerMessageActive == true)
         {
            rc_Data.s32_Enabled = static_cast<int32_t>(Qt::Checked);
         }
         else
         {
            rc_Data.s32_Enabled = static_cast<int32_t>(Qt::Unchecked);
         }

         if (pc_Message->q_IsExtended == true)
         {
            rc_Data.s32_Extended = static_cast<int32_t>(Qt::Checked);
         }
         else
         {
            rc_Data.s32_Extended = static_cast<int32_t>(Qt::Unchecked);
         }

         rc_Data.c_CobId = this->m_GetCobId(*pc_Message, 0);
         rc_Data.c_CobIdEditRole = this->m_GetCobId(*pc_Message, static_cast<int32_t>(Qt::EditRole));
         rc_Data.c_CanId = "0x" + QString::number(pc_Message->u32_CanId, 16).toUpper();
         rc_Data.c_CanIdEditRole = QString::number(pc_Message->u32_CanId);
         rc_Data.u16_Dlc = pc_Message->u16_Dlc;
         rc_Data.c_TxMethod = C_SdUtil::h_ConvertTxMethodToName(pc_Message->e_TxMethod);

         if (pc_Message->IsTransmissionTypeOfCyclicType())
         {
            rc_Data.c_CycleTime = QString::number(pc_Message->u32_CycleTimeMs);
         }
         else
         {
            rc_Data.c_CycleTime = "-";
         }

         rc_Data.c_NotEarlierThan = this->m_GetNotEarlierThanValue(*pc_Message);
         rc_Data.c_NotLaterThan = this->m_GetNotLaterThanValue(*pc_Message);
         rc_Data.c_Transmitter = this->m_GetTransmitter(rc_Id, *pc_Message);
         rc_Data.c_Receiver = this->m_GetReceiver(rc_Id, *pc_Message);
         rc_Data.c_Icon = this->m_GetMessageIcon(rc_Id);

         this->m_FillJ1939MsgInfo(rc_Data, *pc_Message);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Fill j1939 message info

   \param[in,out]  orc_MessageTableData   Message table data
   \param[in]      orc_MessageData        Message data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageTableModel::m_FillJ1939MsgInfo(C_SdBueMessageTableModel::C_MsgTableData & orc_MessageTableData,
                                                  const C_OscCanMessage & orc_MessageData)
{
   C_OscCanUtilJ1939PgInfo c_PgInfo;

   C_OscCanUtil::h_GetJ1939PgInfoFromCanId(orc_MessageData.u32_CanId, c_PgInfo);

   orc_MessageTableData.c_J1939Pgn = C_OscCanUtil::h_GetVisiblePgn(c_PgInfo.u32_Pgn);
   orc_MessageTableData.c_J1939Priority = static_cast<uint32_t>(c_PgInfo.u8_Priority);
   orc_MessageTableData.c_J1939SourceAddress = static_cast<uint32_t>(c_PgInfo.u8_SourceAddress);
   if (c_PgInfo.q_HasDestinationAddress)
   {
      orc_MessageTableData.c_J1939DestinationAddress = QString::number(c_PgInfo.u8_PduSpecific);
      orc_MessageTableData.c_J1939Format = C_GtGetText::h_GetText("PDU 1");
   }
   else
   {
      orc_MessageTableData.c_J1939DestinationAddress = C_GtGetText::h_GetText("All");
      orc_MessageTableData.c_J1939Format = C_GtGetText::h_GetText("PDU 2");
   }
   orc_MessageTableData.c_J1939EdpAndDp = QString::number(c_PgInfo.u8_Edp) + QString::number(c_PgInfo.u8_Dp);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get "Cycle Time" header name

   \retval
    "Cycle Time" header name
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdBueMessageTableModel::m_GetCycleTimeHeaderName(void) const
{
   QString c_Retval = C_GtGetText::h_GetText("Cycle Time [ms]");

   if (this->mpc_SyncManager != NULL)
   {
      if (this->mpc_SyncManager->GetCurrentComProtocol() == C_OscCanProtocol::eCAN_OPEN_SAFETY)
      {
         c_Retval = C_GtGetText::h_GetText("Safety Cycle-Time [ms]");
      }
   }
   return c_Retval;
}
