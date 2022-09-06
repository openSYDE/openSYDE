//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Signal table model (implementation)

   Signal table model

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "constants.h"
#include "C_SdBueSignalTableModel.h"
#include "C_GtGetText.h"
#include "C_PuiSdHandler.h"
#include "C_SdUtil.h"
#include "C_SdTooltipUtil.h"
#include "C_SdNdeDpContentUtil.h"
#include "TGLUtils.h"
#include "C_SdBueCoAddSignalsModel.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
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
C_SdBueSignalTableModel::C_SdBueSignalTableModel(QObject * const opc_Parent) :
   QAbstractTableModel(opc_Parent),
   mpc_SyncManager(NULL)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set message sync manager

   \param[in,out] opc_Value Message sync manager
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueSignalTableModel::SetMessageSyncManager(C_PuiSdNodeCanMessageSyncManager * const opc_Value)
{
   this->mpc_SyncManager = opc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Signal data changes to table
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueSignalTableModel::UpdateData(void)
{
   if (this->mpc_SyncManager != NULL)
   {
      beginResetModel();
      this->m_FillSigInfo();
      endResetModel();
   }
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
QVariant C_SdBueSignalTableModel::headerData(const sintn osn_Section, const Qt::Orientation oe_Orientation,
                                             const sintn osn_Role) const
{
   QVariant c_Retval = QAbstractTableModel::headerData(osn_Section, oe_Orientation, osn_Role);

   if (oe_Orientation == Qt::Orientation::Horizontal)
   {
      const C_SdBueSignalTableModel::E_Columns e_Col = h_ColumnToEnum(osn_Section);
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
         case eMESSAGE:
            c_Retval = C_GtGetText::h_GetText("Message");
            break;
         case eNAME:
            c_Retval = C_GtGetText::h_GetText("Name");
            break;
         case eCOMMENT:
            c_Retval = C_GtGetText::h_GetText("Comment");
            break;
         case eCAN_OPEN_INDEX:
            c_Retval = C_GtGetText::h_GetText("Index");
            break;
         case eMULTIPLEXING:
            c_Retval = C_GtGetText::h_GetText("Multiplexing");
            break;
         case eSTART_BIT:
            c_Retval = C_GtGetText::h_GetText("Start [Bit]");
            break;
         case eLENGTH:
            c_Retval = C_GtGetText::h_GetText("Length [Bit]");
            break;
         case eBYTE_ORDER:
            c_Retval = C_GtGetText::h_GetText("Byte order");
            break;
         case eVALUE_TYPE:
            c_Retval = C_GtGetText::h_GetText("Value type");
            break;
         case eINITIAL_VALUE:
            c_Retval = C_GtGetText::h_GetText("Initial value");
            break;
         case eFACTOR:
            c_Retval = C_GtGetText::h_GetText("Factor");
            break;
         case eOFFSET:
            c_Retval = C_GtGetText::h_GetText("Offset");
            break;
         case eAUTO_MIN_MAX:
            c_Retval = C_GtGetText::h_GetText("Auto min/max");
            break;
         case eMINIMUM_VALUE:
            c_Retval = C_GtGetText::h_GetText("Minimum");
            break;
         case eMAXIMUM_VALUE:
            c_Retval = C_GtGetText::h_GetText("Maximum");
            break;
         case eUNIT:
            c_Retval = C_GtGetText::h_GetText("Unit");
            break;
         default:
            tgl_assert(false);
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
sintn C_SdBueSignalTableModel::rowCount(const QModelIndex & orc_Parent) const
{
   stw_types::sintn sn_Retval = 0;
   if (!orc_Parent.isValid())
   {
      sn_Retval = this->mc_SigInfoAll.size();
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
sintn C_SdBueSignalTableModel::columnCount(const QModelIndex & orc_Parent) const
{
   sintn sn_Retval = 0;

   if (!orc_Parent.isValid())
   {
      //For table parent should always be invalid
      sn_Retval = 18;
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
QVariant C_SdBueSignalTableModel::data(const QModelIndex & orc_Index, const sintn osn_Role) const
{
   QVariant c_Retval;

   if (orc_Index.isValid() == true)
   {
      const C_SdBueSignalTableModel::E_Columns e_Col = h_ColumnToEnum(orc_Index.column());
      if ((osn_Role == static_cast<sintn>(Qt::DisplayRole)) || (osn_Role == static_cast<sintn>(Qt::EditRole)))
      {
         c_Retval = this->m_GetDisplayAndEditValue(orc_Index, e_Col);
      }
      else if (osn_Role == static_cast<sintn>(Qt::CheckStateRole))
      {
         if (e_Col == eAUTO_MIN_MAX)
         {
            const sintn sn_Index = orc_Index.row();
            if ((static_cast<uintn>(sn_Index) < this->mc_SigInfoAll.size()) && (sn_Index >= 0))
            {
               c_Retval = this->mc_SigInfoAll[sn_Index].c_SignalData.sn_AutoMinMax;
            }
         }
      }
      else if (osn_Role == msn_USER_ROLE_ICON)
      {
         switch (e_Col)
         {
         case eICON:
            {
               const sintn sn_Index = orc_Index.row();
               if ((static_cast<uintn>(sn_Index) < this->mc_SigInfoAll.size()) && (sn_Index >= 0))
               {
                  c_Retval = this->mc_SigInfoAll[sn_Index].c_SignalData.c_Icon;
               }
               break;
            }
         case eMESSAGE:
         case eNAME:
         case eCOMMENT:
         case eLENGTH:
         case eBYTE_ORDER:
         case eVALUE_TYPE:
         case eINITIAL_VALUE:
         case eFACTOR:
         case eOFFSET:
         case eMULTIPLEXING:
         case eAUTO_MIN_MAX:
         case eMINIMUM_VALUE:
         case eMAXIMUM_VALUE:
         case eUNIT:
         case eINDEX:
         case eSTART_BIT:
         case eCAN_OPEN_INDEX:
            //No decoration
            break;
         default:
            tgl_assert(false);
            break;
         }
      }
      else if (osn_Role == static_cast<sintn>(Qt::FontRole))
      {
         c_Retval = this->m_GetFontValue(e_Col);
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
         //No action required
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
Qt::ItemFlags C_SdBueSignalTableModel::flags(const QModelIndex & orc_Index) const
{
   Qt::ItemFlags c_Retval;
   if (orc_Index.isValid() == true)
   {
      const C_SdBueSignalTableModel::E_Columns e_Col = C_SdBueSignalTableModel::h_ColumnToEnum(orc_Index.column());
      //Each item
      c_Retval = QAbstractTableModel::flags(orc_Index);
      //Add edit
      if (e_Col == eAUTO_MIN_MAX)
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
C_SdBueSignalTableModel::E_Columns C_SdBueSignalTableModel::h_ColumnToEnum(const sint32 & ors32_Column)
{
   C_SdBueSignalTableModel::E_Columns e_Retval = eNAME;
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
      e_Retval = eCAN_OPEN_INDEX;
      break;
   case 5:
      e_Retval = eMESSAGE;
      break;
   case 6:
      e_Retval = eMULTIPLEXING;
      break;
   case 7:
      e_Retval = eSTART_BIT;
      break;
   case 8:
      e_Retval = eLENGTH;
      break;
   case 9:
      e_Retval = eBYTE_ORDER;
      break;
   case 10:
      e_Retval = eVALUE_TYPE;
      break;
   case 11:
      e_Retval = eINITIAL_VALUE;
      break;
   case 12:
      e_Retval = eFACTOR;
      break;
   case 13:
      e_Retval = eOFFSET;
      break;
   case 14:
      e_Retval = eAUTO_MIN_MAX;
      break;
   case 15:
      e_Retval = eMINIMUM_VALUE;
      break;
   case 16:
      e_Retval = eMAXIMUM_VALUE;
      break;
   case 17:
      e_Retval = eUNIT;
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
sint32 C_SdBueSignalTableModel::h_EnumToColumn(const C_SdBueSignalTableModel::E_Columns & ore_Value)
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
   case eCAN_OPEN_INDEX:
      s32_Retval = 4;
      break;
   case eMESSAGE:
      s32_Retval = 5;
      break;
   case eMULTIPLEXING:
      s32_Retval = 6;
      break;
   case eSTART_BIT:
      s32_Retval = 7;
      break;
   case eLENGTH:
      s32_Retval = 8;
      break;
   case eBYTE_ORDER:
      s32_Retval = 9;
      break;
   case eVALUE_TYPE:
      s32_Retval = 10;
      break;
   case eINITIAL_VALUE:
      s32_Retval = 11;
      break;
   case eFACTOR:
      s32_Retval = 12;
      break;
   case eOFFSET:
      s32_Retval = 13;
      break;
   case eAUTO_MIN_MAX:
      s32_Retval = 14;
      break;
   case eMINIMUM_VALUE:
      s32_Retval = 15;
      break;
   case eMAXIMUM_VALUE:
      s32_Retval = 16;
      break;
   case eUNIT:
      s32_Retval = 17;
      break;
   default:
      s32_Retval = -1;
      break;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert row to signal

   \param[in]   ors32_Row        Model row
   \param[out]  orc_MessageId    Message identification indices
   \param[out]  oru32_Signal     Signal index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdBueSignalTableModel::ConvertRowToSignal(const sint32 & ors32_Row,
                                                   C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                   uint32 & oru32_Signal) const
{
   sint32 s32_Retval = C_NO_ERR;

   if ((static_cast<uintn>(ors32_Row) < this->mc_SigInfoAll.size()) && (ors32_Row >= 0))
   {
      orc_MessageId = this->mc_SigInfoAll[ors32_Row].c_MessageId;
      oru32_Signal = this->mc_SigInfoAll[ors32_Row].u32_SignalIndex;
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get multiplexing value

   \param[in]  orc_MessageId  Message id
   \param[in]  ou32_Signal    Signal

   \return
   Multiplexing value
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdBueSignalTableModel::m_GetMultiplexingValue(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                        const uint32 ou32_Signal) const
{
   QString c_Retval;
   const C_OSCCanSignal * const pc_OSCSignal =
      C_PuiSdHandler::h_GetInstance()->GetCanSignal(orc_MessageId, ou32_Signal);

   if (pc_OSCSignal != NULL)
   {
      const C_OSCCanMessage * pc_Message;
      QString c_MultiplexerName = C_GtGetText::h_GetText("<no multiplexer defined yet>");
      uint32 u32_Multiplexer;
      switch (pc_OSCSignal->e_MultiplexerType)
      {
      case C_OSCCanSignal::eMUX_DEFAULT:
         c_Retval = "-";
         break;
      case C_OSCCanSignal::eMUX_MULTIPLEXER_SIGNAL:
         c_Retval = C_GtGetText::h_GetText("Multiplexer");
         break;
      case C_OSCCanSignal::eMUX_MULTIPLEXED_SIGNAL:
         // find out name of multiplexer
         pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(orc_MessageId);
         if (pc_Message != NULL)
         {
            if (pc_Message->IsMultiplexed(&u32_Multiplexer) == true)
            {
               // multiplexer found -> name is name of corresponding data pool list element
               const C_OSCNodeDataPoolListElement * const pc_OSCSignalCommon =
                  C_PuiSdHandler::h_GetInstance()->GetOSCCanDataPoolListElement(orc_MessageId,
                                                                                u32_Multiplexer);
               if (pc_OSCSignalCommon != NULL)
               {
                  c_MultiplexerName = static_cast<QString>(pc_OSCSignalCommon->c_Name.c_str());
               }
            } // if no multiplexer is found dummy-text is used

            c_Retval = static_cast<QString>(C_GtGetText::h_GetText("%1=%2")).arg(c_MultiplexerName).arg(
               pc_OSCSignal->u16_MultiplexValue);
         }
         break;
      default:
         tgl_assert(false);
         break;
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get display and edit value

   \param[in]  orc_Index   Index
   \param[in]  oe_Column   Column

   \return
   Display and edit value
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdBueSignalTableModel::m_GetDisplayAndEditValue(const QModelIndex & orc_Index,
                                                           const C_SdBueSignalTableModel::E_Columns oe_Column) const
{
   QVariant c_Retval;

   const sintn sn_Index = orc_Index.row();

   if ((static_cast<uintn>(sn_Index) < this->mc_SigInfoAll.size()) && (sn_Index >= 0))
   {
      switch (oe_Column)
      {
      case eINDEX:
         c_Retval = orc_Index.row() + 1;
         break;
      case eICON:
         c_Retval = "";
         break;
      case eMESSAGE:
         c_Retval = this->mc_SigInfoAll[sn_Index].c_SignalData.c_MsgName;
         break;
      case eCAN_OPEN_INDEX:
         c_Retval = this->mc_SigInfoAll[sn_Index].c_SignalData.c_CoIndex;
         break;
      case eMULTIPLEXING:
         c_Retval = this->mc_SigInfoAll[sn_Index].c_SignalData.c_MultiplexingValue;
         break;
      case eNAME:
         c_Retval = this->mc_SigInfoAll[sn_Index].c_SignalData.c_SigName;
         break;
      case eCOMMENT:
         c_Retval = this->mc_SigInfoAll[sn_Index].c_SignalData.c_Comment;
         break;
      case eSTART_BIT:
         c_Retval = this->mc_SigInfoAll[sn_Index].c_SignalData.u16_StartBit;
         break;
      case eLENGTH:
         c_Retval = this->mc_SigInfoAll[sn_Index].c_SignalData.u16_BitLength;
         break;
      case eBYTE_ORDER:
         c_Retval = this->mc_SigInfoAll[sn_Index].c_SignalData.c_ByteOrder;
         break;
      case eVALUE_TYPE:
         c_Retval = this->mc_SigInfoAll[sn_Index].c_SignalData.c_ValueType;
         break;
      case eINITIAL_VALUE:
         c_Retval = this->mc_SigInfoAll[sn_Index].c_SignalData.c_InitValue;
         break;
      case eFACTOR:
         c_Retval = this->mc_SigInfoAll[sn_Index].c_SignalData.c_Factor;
         break;
      case eOFFSET:
         c_Retval = this->mc_SigInfoAll[sn_Index].c_SignalData.c_Offset;
         break;
      case eAUTO_MIN_MAX:
         c_Retval = "";
         break;
      case eMINIMUM_VALUE:
         c_Retval = this->mc_SigInfoAll[sn_Index].c_SignalData.c_MinValue;
         break;
      case eMAXIMUM_VALUE:
         c_Retval = this->mc_SigInfoAll[sn_Index].c_SignalData.c_MaxValue;
         break;
      case eUNIT:
         c_Retval = this->mc_SigInfoAll[sn_Index].c_SignalData.c_Unit;
         break;
      default:
         tgl_assert(false);
         break;
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get font value

   \param[in]  oe_Column   Column

   \return
   Font value
*/
//----------------------------------------------------------------------------------------------------------------------
QFont C_SdBueSignalTableModel::m_GetFontValue(const C_SdBueSignalTableModel::E_Columns oe_Column) const
{
   QFont c_Font;

   //Stylesheets do not allow access of specific columns so we need to set fonts manually
   switch (oe_Column)
   {
   case eCOMMENT:
      c_Font = mc_STYLE_GUIDE_FONT_REGULAR_12;
      break;
   case eICON:
   case eINDEX:
   case eSTART_BIT:
   case eMESSAGE:
   case eNAME:
   case eLENGTH:
   case eBYTE_ORDER:
   case eVALUE_TYPE:
   case eMULTIPLEXING:
   case eINITIAL_VALUE:
   case eFACTOR:
   case eOFFSET:
   case eAUTO_MIN_MAX:
   case eMINIMUM_VALUE:
   case eMAXIMUM_VALUE:
   case eCAN_OPEN_INDEX:
   case eUNIT:
      c_Font = mc_STYLE_GUIDE_FONT_REGULAR_14;
      break;
   default:
      tgl_assert(false);
      break;
   }
   //Convert point to pixel
   c_Font.setPixelSize(c_Font.pointSize());
   return c_Font;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Collects all signal data for overview table
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueSignalTableModel::m_FillSigInfo(void)
{
   const std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> c_MessageIds =
      this->mpc_SyncManager->GetUniqueMessages();
   uint32 u32_MessageCounter;

   this->mc_SigInfoAll.clear();

   for (u32_MessageCounter = 0U; u32_MessageCounter < c_MessageIds.size(); ++u32_MessageCounter)
   {
      const C_OSCCanMessage * const pc_Message =
         C_PuiSdHandler::h_GetInstance()->GetCanMessage(c_MessageIds[u32_MessageCounter]);
      if (pc_Message != NULL)
      {
         uint32 u32_SignalCounter;
         C_SigTableConfig c_SigConfig;
         const C_OSCCanMessageIdentificationIndices & rc_MsgId = c_MessageIds[u32_MessageCounter];

         c_SigConfig.c_MessageId = rc_MsgId;

         for (u32_SignalCounter = 0U; u32_SignalCounter < pc_Message->c_Signals.size(); ++u32_SignalCounter)
         {
            const C_OSCCanSignal * const pc_OSCSignal =
               C_PuiSdHandler::h_GetInstance()->GetCanSignal(rc_MsgId, u32_SignalCounter);
            const C_OSCNodeDataPoolListElement * const pc_OSCSignalCommon =
               C_PuiSdHandler::h_GetInstance()->GetOSCCanDataPoolListElement(rc_MsgId,
                                                                             u32_SignalCounter);
            const C_PuiSdNodeDataPoolListElement * const pc_UiSignalCommon =
               C_PuiSdHandler::h_GetInstance()->GetUiCanDataPoolListElement(rc_MsgId, u32_SignalCounter);

            const C_OSCNodeDataPoolList * const pc_List =
               C_PuiSdHandler::h_GetInstance()->GetOSCCanDataPoolList(
                  rc_MsgId.u32_NodeIndex,
                  rc_MsgId.e_ComProtocol,
                  rc_MsgId.u32_InterfaceIndex,
                  rc_MsgId.u32_DatapoolIndex,
                  rc_MsgId.q_MessageIsTx);

            if ((pc_OSCSignal != NULL) &&
                (pc_OSCSignalCommon != NULL) &&
                (pc_UiSignalCommon != NULL) &&
                (pc_List != NULL))
            {
               C_SigTableData c_SigData;
               const bool q_SignalValid = !pc_Message->CheckErrorSignal(
                  pc_List, u32_SignalCounter,
                  C_OSCCanProtocol::h_GetCANMessageValidSignalsDLCOffset(rc_MsgId.e_ComProtocol),
                  C_OSCCanProtocol::h_GetCANMessageSignalGapsValid(rc_MsgId.e_ComProtocol),
                  C_OSCCanProtocol::h_GetCANMessageSignalByteAlignmentRequired(rc_MsgId.e_ComProtocol));
               QStringList c_TmpIcon;

               // Fill the signal data
               c_SigData.c_MsgName = static_cast<QString>("%1 (0x%2)").arg(pc_Message->c_Name.c_str(),
                                                                           QString::number(pc_Message->u32_CanId,
                                                                                           16).toUpper());
               c_SigData.c_CoIndex = C_SdUtil::h_GetCanOpenSignalObjectIndex(
                  pc_OSCSignal->u16_CanOpenManagerObjectDictionaryIndex,
                  pc_OSCSignal->u8_CanOpenManagerObjectDictionarySubIndex);

               c_SigData.c_MultiplexingValue = this->m_GetMultiplexingValue(rc_MsgId, u32_SignalCounter);
               c_SigData.c_SigName = static_cast<QString>(pc_OSCSignalCommon->c_Name.c_str());
               c_SigData.c_Comment = static_cast<QString>(pc_OSCSignalCommon->c_Comment.c_str());
               c_SigData.u16_StartBit = pc_OSCSignal->u16_ComBitStart;
               c_SigData.u16_BitLength = pc_OSCSignal->u16_ComBitLength;
               c_SigData.c_ByteOrder = C_SdUtil::h_ConvertByteOrderToName(pc_OSCSignal->e_ComByteOrder);
               c_SigData.c_ValueType = C_SdTooltipUtil::h_ConvertTypeToNameSimplified(pc_OSCSignalCommon->GetType());
               if (pc_OSCSignalCommon->c_DataSetValues.size() > 0)
               {
                  c_SigData.c_InitValue = C_SdNdeDpContentUtil::h_ConvertScaledContentToGeneric(
                     pc_OSCSignalCommon->c_DataSetValues[0],
                     pc_OSCSignalCommon->f64_Factor,
                     pc_OSCSignalCommon->f64_Offset, 0);
               }
               c_SigData.c_Factor = pc_OSCSignalCommon->f64_Factor;
               c_SigData.c_Offset = pc_OSCSignalCommon->f64_Offset;

               if (pc_UiSignalCommon->q_AutoMinMaxActive == true)
               {
                  c_SigData.sn_AutoMinMax = static_cast<sintn>(Qt::Checked);
               }
               else
               {
                  c_SigData.sn_AutoMinMax = static_cast<sintn>(Qt::Unchecked);
               }

               c_SigData.c_MinValue = C_SdNdeDpContentUtil::h_ConvertScaledContentToGeneric(
                  pc_OSCSignalCommon->c_MinValue,
                  pc_OSCSignalCommon->f64_Factor,
                  pc_OSCSignalCommon->f64_Offset,
                  0);
               c_SigData.c_MaxValue = C_SdNdeDpContentUtil::h_ConvertScaledContentToGeneric(
                  pc_OSCSignalCommon->c_MaxValue,
                  pc_OSCSignalCommon->f64_Factor,
                  pc_OSCSignalCommon->f64_Offset,
                  0);
               c_SigData.c_Unit = static_cast<QString>(pc_OSCSignalCommon->c_Unit.c_str());

               // The icon
               c_TmpIcon.push_back(QString::number(20));
               if (q_SignalValid == false)
               {
                  c_TmpIcon.push_back("://images/system_definition/IconSignalError.svg");
               }
               else
               {
                  c_TmpIcon.push_back("://images/system_definition/IconSignal.svg");
               }
               c_SigData.c_Icon = c_TmpIcon;

               // Save the signal config
               c_SigConfig.u32_SignalIndex = u32_SignalCounter;
               c_SigConfig.c_SignalData = c_SigData;
               this->mc_SigInfoAll.push_back(c_SigConfig);
            }
         }
      }
   }
}
