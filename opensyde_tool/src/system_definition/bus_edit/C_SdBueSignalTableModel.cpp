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
   mpc_SyncManager(NULL),
   mu32_SignalCount(0)
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
      this->mc_MessageIds = this->mpc_SyncManager->GetUniqueMessages();
      this->mu32_SignalCount = 0;
      for (uint32 u32_MessageId = 0; u32_MessageId < this->mc_MessageIds.size(); ++u32_MessageId)
      {
         const C_OSCCanMessage * const pc_Message =
            C_PuiSdHandler::h_GetInstance()->GetCanMessage(this->mc_MessageIds[u32_MessageId]);
         if (pc_Message != NULL)
         {
            this->mu32_SignalCount += pc_Message->c_Signals.size();
         }
      }
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
      sn_Retval = this->mu32_SignalCount;
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
sintn C_SdBueSignalTableModel::columnCount(const QModelIndex & orc_Parent) const
{
   sintn sn_Retval = 0;

   if (!orc_Parent.isValid())
   {
      //For table parent should always be invalid
      sn_Retval = 17;
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
QVariant C_SdBueSignalTableModel::data(const QModelIndex & orc_Index, const sintn osn_Role) const
{
   QVariant c_Retval;

   if (orc_Index.isValid() == true)
   {
      const C_SdBueSignalTableModel::E_Columns e_Col = h_ColumnToEnum(orc_Index.column());
      if ((osn_Role == static_cast<sintn>(Qt::DisplayRole)) || (osn_Role == static_cast<sintn>(Qt::EditRole)))
      {
         C_OSCCanMessageIdentificationIndices c_MessageId;
         uint32 u32_SignalIndex = 0U;
         if (this->ConvertRowToSignal(orc_Index.row(), c_MessageId, u32_SignalIndex) == C_NO_ERR)
         {
            const C_OSCCanMessage * pc_Message;
            const C_OSCCanSignal * pc_OSCSignal;
            const C_OSCNodeDataPoolListElement * pc_OSCSignalCommon;

            switch (e_Col)
            {
            case eINDEX:
               c_Retval = orc_Index.row() + 1;
               break;
            case eICON:
               c_Retval = "";
               break;
            case eMESSAGE:
               pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(c_MessageId);
               if (pc_Message != NULL)
               {
                  c_Retval =
                     static_cast<QString>("%1 (0x%2)").arg(pc_Message->c_Name.c_str(),
                                                           QString::number(pc_Message->u32_CanId, 16));
               }
               break;
            case eMULTIPLEXING:
               pc_OSCSignal = C_PuiSdHandler::h_GetInstance()->GetCanSignal(c_MessageId, u32_SignalIndex);
               if (pc_OSCSignal != NULL)
               {
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
                     pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(c_MessageId);
                     if (pc_Message != NULL)
                     {
                        if (pc_Message->IsMultiplexed(&u32_Multiplexer) == true)
                        {
                           // multiplexer found -> name is name of corresponding data pool list element
                           pc_OSCSignalCommon =
                              C_PuiSdHandler::h_GetInstance()->GetOSCCanDataPoolListElement(c_MessageId,
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
               break;
            case eNAME:
               pc_OSCSignalCommon =
                  C_PuiSdHandler::h_GetInstance()->GetOSCCanDataPoolListElement(c_MessageId, u32_SignalIndex);
               if (pc_OSCSignalCommon != NULL)
               {
                  c_Retval = static_cast<QString>(pc_OSCSignalCommon->c_Name.c_str());
               }
               break;
            case eCOMMENT:
               pc_OSCSignalCommon =
                  C_PuiSdHandler::h_GetInstance()->GetOSCCanDataPoolListElement(c_MessageId, u32_SignalIndex);
               if (pc_OSCSignalCommon != NULL)
               {
                  c_Retval = static_cast<QString>(pc_OSCSignalCommon->c_Comment.c_str());
               }
               break;
            case eSTART_BIT:
               pc_OSCSignal = C_PuiSdHandler::h_GetInstance()->GetCanSignal(c_MessageId, u32_SignalIndex);
               if (pc_OSCSignal != NULL)
               {
                  c_Retval = pc_OSCSignal->u16_ComBitStart;
               }
               break;
            case eLENGTH:
               pc_OSCSignal = C_PuiSdHandler::h_GetInstance()->GetCanSignal(c_MessageId, u32_SignalIndex);
               if (pc_OSCSignal != NULL)
               {
                  c_Retval = pc_OSCSignal->u16_ComBitLength;
               }
               break;
            case eBYTE_ORDER:
               pc_OSCSignal = C_PuiSdHandler::h_GetInstance()->GetCanSignal(c_MessageId, u32_SignalIndex);
               if (pc_OSCSignal != NULL)
               {
                  c_Retval = C_SdUtil::h_ConvertByteOrderToName(pc_OSCSignal->e_ComByteOrder);
               }
               break;
            case eVALUE_TYPE:
               pc_OSCSignalCommon =
                  C_PuiSdHandler::h_GetInstance()->GetOSCCanDataPoolListElement(c_MessageId, u32_SignalIndex);
               if (pc_OSCSignalCommon != NULL)
               {
                  c_Retval = C_SdTooltipUtil::h_ConvertTypeToNameSimplified(pc_OSCSignalCommon->GetType());
               }
               break;
            case eINITIAL_VALUE:
               pc_OSCSignalCommon =
                  C_PuiSdHandler::h_GetInstance()->GetOSCCanDataPoolListElement(c_MessageId, u32_SignalIndex);
               if (pc_OSCSignalCommon != NULL)
               {
                  if (pc_OSCSignalCommon->c_DataSetValues.size() > 0)
                  {
                     c_Retval = C_SdNdeDpContentUtil::h_ConvertScaledContentToGeneric(
                        pc_OSCSignalCommon->c_DataSetValues[0],
                        pc_OSCSignalCommon->f64_Factor,
                        pc_OSCSignalCommon->f64_Offset, 0);
                  }
               }
               break;
            case eFACTOR:
               pc_OSCSignalCommon =
                  C_PuiSdHandler::h_GetInstance()->GetOSCCanDataPoolListElement(c_MessageId, u32_SignalIndex);
               if (pc_OSCSignalCommon != NULL)
               {
                  c_Retval = pc_OSCSignalCommon->f64_Factor;
               }
               break;
            case eOFFSET:
               pc_OSCSignalCommon =
                  C_PuiSdHandler::h_GetInstance()->GetOSCCanDataPoolListElement(c_MessageId, u32_SignalIndex);
               if (pc_OSCSignalCommon != NULL)
               {
                  c_Retval = pc_OSCSignalCommon->f64_Offset;
               }
               break;
            case eAUTO_MIN_MAX:
               c_Retval = "";
               break;
            case eMINIMUM_VALUE:
               pc_OSCSignalCommon =
                  C_PuiSdHandler::h_GetInstance()->GetOSCCanDataPoolListElement(c_MessageId, u32_SignalIndex);
               if (pc_OSCSignalCommon != NULL)
               {
                  c_Retval = C_SdNdeDpContentUtil::h_ConvertScaledContentToGeneric(pc_OSCSignalCommon->c_MinValue,
                                                                                   pc_OSCSignalCommon->f64_Factor,
                                                                                   pc_OSCSignalCommon->f64_Offset,
                                                                                   0);
               }
               break;
            case eMAXIMUM_VALUE:
               pc_OSCSignalCommon =
                  C_PuiSdHandler::h_GetInstance()->GetOSCCanDataPoolListElement(c_MessageId, u32_SignalIndex);
               if (pc_OSCSignalCommon != NULL)
               {
                  c_Retval = C_SdNdeDpContentUtil::h_ConvertScaledContentToGeneric(pc_OSCSignalCommon->c_MaxValue,
                                                                                   pc_OSCSignalCommon->f64_Factor,
                                                                                   pc_OSCSignalCommon->f64_Offset,
                                                                                   0);
               }
               break;
            case eUNIT:
               pc_OSCSignalCommon =
                  C_PuiSdHandler::h_GetInstance()->GetOSCCanDataPoolListElement(c_MessageId, u32_SignalIndex);
               if (pc_OSCSignalCommon != NULL)
               {
                  c_Retval = static_cast<QString>(pc_OSCSignalCommon->c_Unit.c_str());
               }
               break;
            default:
               tgl_assert(false);
               break;
            }
         }
      }
      else if (osn_Role == static_cast<sintn>(Qt::CheckStateRole))
      {
         C_OSCCanMessageIdentificationIndices c_MessageId;
         uint32 u32_SignalIndex = 0U;
         if (e_Col == eAUTO_MIN_MAX)
         {
            if (this->ConvertRowToSignal(orc_Index.row(), c_MessageId, u32_SignalIndex) == C_NO_ERR)
            {
               const C_PuiSdNodeDataPoolListElement * const pc_UiSignalCommon =
                  C_PuiSdHandler::h_GetInstance()->GetUiCanDataPoolListElement(c_MessageId, u32_SignalIndex);
               if (pc_UiSignalCommon != NULL)
               {
                  if (pc_UiSignalCommon->q_AutoMinMaxActive == true)
                  {
                     c_Retval = static_cast<sintn>(Qt::Checked);
                  }
                  else
                  {
                     c_Retval = static_cast<sintn>(Qt::Unchecked);
                  }
               }
            }
         }
      }
      else if (osn_Role == msn_USER_ROLE_ICON)
      {
         switch (e_Col)
         {
         case eICON:
            if (orc_Index.isValid() == true)
            {
               C_OSCCanMessageIdentificationIndices c_MessageId;
               uint32 u32_SignalIndex;
               if (this->ConvertRowToSignal(orc_Index.row(), c_MessageId, u32_SignalIndex) == C_NO_ERR)
               {
                  const C_OSCCanMessage * const pc_Message =
                     C_PuiSdHandler::h_GetInstance()->GetCanMessage(c_MessageId);
                  if (pc_Message != NULL)
                  {
                     const C_OSCNodeDataPoolList * const pc_List =
                        C_PuiSdHandler::h_GetInstance()->GetOSCCanDataPoolList(
                           c_MessageId.u32_NodeIndex,
                           c_MessageId.e_ComProtocol,
                           c_MessageId.u32_InterfaceIndex,
                           c_MessageId.u32_DatapoolIndex,
                           c_MessageId.q_MessageIsTx);
                     QStringList c_Tmp;
                     const bool q_SignalValid = !pc_Message->CheckErrorSignal(
                        pc_List, u32_SignalIndex,
                        C_OSCCanProtocol::h_GetCANMessageValidSignalsDLCOffset(c_MessageId.e_ComProtocol),
                        C_OSCCanProtocol::h_GetCANMessageSignalGapsValid(c_MessageId.e_ComProtocol));
                     c_Tmp.push_back(QString::number(20));
                     if (q_SignalValid == false)
                     {
                        c_Tmp.push_back("://images/system_definition/IconSignalError.svg");
                     }
                     else
                     {
                        c_Tmp.push_back("://images/system_definition/IconSignal.svg");
                     }
                     c_Retval = c_Tmp;
                  }
               }
            }
            break;
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
            //No decoration
            break;
         default:
            tgl_assert(false);
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
         case eUNIT:
            c_Font = mc_STYLE_GUIDE_FONT_REGULAR_14;
            break;
         default:
            tgl_assert(false);
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

   \param[in]  ors32_Column Column

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
      e_Retval = eMESSAGE;
      break;
   case 5:
      e_Retval = eMULTIPLEXING;
      break;
   case 6:
      e_Retval = eSTART_BIT;
      break;
   case 7:
      e_Retval = eLENGTH;
      break;
   case 8:
      e_Retval = eBYTE_ORDER;
      break;
   case 9:
      e_Retval = eVALUE_TYPE;
      break;
   case 10:
      e_Retval = eINITIAL_VALUE;
      break;
   case 11:
      e_Retval = eFACTOR;
      break;
   case 12:
      e_Retval = eOFFSET;
      break;
   case 13:
      e_Retval = eAUTO_MIN_MAX;
      break;
   case 14:
      e_Retval = eMINIMUM_VALUE;
      break;
   case 15:
      e_Retval = eMAXIMUM_VALUE;
      break;
   case 16:
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

   \param[in] ore_Value Enum value

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
   case eMESSAGE:
      s32_Retval = 4;
      break;
   case eMULTIPLEXING:
      s32_Retval = 5;
      break;
   case eSTART_BIT:
      s32_Retval = 6;
      break;
   case eLENGTH:
      s32_Retval = 7;
      break;
   case eBYTE_ORDER:
      s32_Retval = 8;
      break;
   case eVALUE_TYPE:
      s32_Retval = 9;
      break;
   case eINITIAL_VALUE:
      s32_Retval = 10;
      break;
   case eFACTOR:
      s32_Retval = 11;
      break;
   case eOFFSET:
      s32_Retval = 12;
      break;
   case eAUTO_MIN_MAX:
      s32_Retval = 13;
      break;
   case eMINIMUM_VALUE:
      s32_Retval = 14;
      break;
   case eMAXIMUM_VALUE:
      s32_Retval = 15;
      break;
   case eUNIT:
      s32_Retval = 16;
      break;
   default:
      s32_Retval = -1;
      break;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert row to signal

   \param[in]  ors32_Row     Model row
   \param[out] orc_MessageId Message identification indices
   \param[out] oru32_Signal  Signal index

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

   if (ors32_Row >= 0)
   {
      const uint32 u32_Row = static_cast<uint32>(ors32_Row);
      uint32 u32_InternalCounter = 0;
      bool q_Found = false;
      for (uint32 u32_ItMessage = 0; u32_ItMessage < this->mc_MessageIds.size(); ++u32_ItMessage)
      {
         const C_OSCCanMessage * const pc_Message =
            C_PuiSdHandler::h_GetInstance()->GetCanMessage(this->mc_MessageIds[u32_ItMessage]);
         if (pc_Message != NULL)
         {
            const uint32 u32_SignalSize = pc_Message->c_Signals.size();
            if (u32_SignalSize > 0)
            {
               //Check
               if ((u32_InternalCounter <= u32_Row) && (u32_Row < (u32_InternalCounter + u32_SignalSize)))
               {
                  q_Found = true;
                  //Output
                  orc_MessageId = this->mc_MessageIds[u32_ItMessage];
                  oru32_Signal = u32_Row - u32_InternalCounter;
               }
               //Next
               u32_InternalCounter += u32_SignalSize;
            }
         }
      }
      if (q_Found == false)
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
