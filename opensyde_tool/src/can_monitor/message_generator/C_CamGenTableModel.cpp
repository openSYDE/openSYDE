//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Model component for message generator table (implementation)

   Model component for message generator table

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QBitArray>

#include "C_Uti.hpp"
#include "TglUtils.hpp"
#include "constants.hpp"
#include "stwerrors.hpp"
#include "C_GtGetText.hpp"
#include "cam_constants.hpp"
#include "C_CamGenSigUtil.hpp"
#include "C_CamProHandler.hpp"
#include "C_CamGenTableModel.hpp"
#include "C_CamProClipBoardHelper.hpp"
#include "C_CamDbHandler.hpp"
#include "C_OscCanProtocol.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
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
/*! \brief  Default constructor

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamGenTableModel::C_CamGenTableModel(QObject * const opc_Parent) :
   C_TblModelAction(opc_Parent)
{
   connect(C_CamProHandler::h_GetInstance(), &C_CamProHandler::SigNewConfiguration, this,
           &C_CamGenTableModel::ReloadAll);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update message key column

   \param[in]  ou32_MessageIndex    Message index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableModel::UpdateMessageKey(const uint32_t ou32_MessageIndex)
{
   QVector<int32_t> c_Roles;
   const int32_t s32_Col = C_CamGenTableModel::h_EnumToColumn(C_CamGenTableModel::eKEY);

   c_Roles.push_back(static_cast<int32_t>(Qt::DisplayRole));

   Q_EMIT (this->dataChanged(this->index(ou32_MessageIndex, s32_Col),
                             this->index(ou32_MessageIndex, s32_Col), c_Roles));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update message data column

   \param[in]  ou32_MessageIndex    Message index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableModel::UpdateMessageData(const uint32_t ou32_MessageIndex)
{
   QVector<int32_t> c_Roles;
   const int32_t s32_Col = C_CamGenTableModel::h_EnumToColumn(C_CamGenTableModel::eDATA);

   c_Roles.push_back(static_cast<int32_t>(Qt::DisplayRole));

   Q_EMIT (this->dataChanged(this->index(ou32_MessageIndex, s32_Col),
                             this->index(ou32_MessageIndex, s32_Col), c_Roles));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update Auto protocol column data

   \param[in]  ou32_MessageIndex    Message index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableModel::UpdateAutoProtocolCellData(const uint32_t ou32_MessageIndex)
{
   QVector<int32_t> c_Roles;
   const int32_t s32_Col = C_CamGenTableModel::h_EnumToColumn(C_CamGenTableModel::eAUTO_SUPPORT);

   c_Roles.push_back(static_cast<int32_t>(Qt::DisplayRole));

   Q_EMIT (this->dataChanged(this->index(ou32_MessageIndex, s32_Col),
                             this->index(ou32_MessageIndex, s32_Col), c_Roles));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Request trigger of model function for updating message AutoProtocol
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableModel::TriggerMessageReload()
{
   const int32_t s32_Col = C_CamGenTableModel::h_EnumToColumn(C_CamGenTableModel::eAUTO_SUPPORT);
   const int32_t s32_RowCount = this->rowCount();
   const QModelIndex c_TopLeft = this->index(0, s32_Col);
   const QModelIndex c_BottomRight = this->index(s32_RowCount - 1, s32_Col);

   QVector<int32_t> c_Roles;
   c_Roles.push_back(static_cast<int32_t>(Qt::DisplayRole));

   Q_EMIT this->dataChanged(c_TopLeft, c_BottomRight, c_Roles);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Request trigger of model function for update cyclic message

   Reason: have one central point for each check which has to be done when changing an existing message

   \param[in]  ou32_MessageIndex    Message index
   \param[in]  oq_Active            Change of cyclic message state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableModel::TriggerModelUpdateCyclicMessage(const uint32_t ou32_MessageIndex, const bool oq_Active)
{
   this->m_CheckAndHandleRegisterCyclicMessage(ou32_MessageIndex, oq_Active);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get header data

   \param[in]  os32_Section       Section
   \param[in]  oe_Orientation    Orientation
   \param[in]  os32_Role          Role

   \return
   Header string
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_CamGenTableModel::headerData(const int32_t os32_Section, const Qt::Orientation oe_Orientation,
                                        const int32_t os32_Role) const
{
   QVariant c_Retval = QAbstractTableModel::headerData(os32_Section, oe_Orientation, os32_Role);

   if (oe_Orientation == Qt::Orientation::Horizontal)
   {
      const C_CamGenTableModel::E_Columns e_Col = h_ColumnToEnum(os32_Section);
      if (os32_Role == static_cast<int32_t>(Qt::DisplayRole))
      {
         QString c_Header;
         switch (e_Col)
         {
         case eRTR:
            c_Header = C_GtGetText::h_GetText("RTR");
            break;
         case eNAME:
            c_Header = C_GtGetText::h_GetText("Name");
            break;
         case eID:
            c_Header = C_GtGetText::h_GetText("ID");
            break;
         case eDLC:
            c_Header = C_GtGetText::h_GetText("DLC");
            break;
         case eDATA:
            c_Header = C_GtGetText::h_GetText("Data");
            break;
         case eCYCLIC_TRIGGER:
            c_Header = C_GtGetText::h_GetText("Cyclic");
            break;
         case eCYCLIC_TIME:
            c_Header = C_GtGetText::h_GetText("Cycle Time");
            break;
         case eMANUAL_TRIGGER:
            c_Header = C_GtGetText::h_GetText("Manual Trigger");
            break;
         case eXTD:
            c_Header = C_GtGetText::h_GetText("XTD");
            break;
         case eKEY:
            c_Header = C_GtGetText::h_GetText("Key");
            break;
         case eAUTO_SUPPORT:
            c_Header = C_GtGetText::h_GetText("Auto Protocol");
            break;
         default:
            tgl_assert(false);
            break;
         }
         //Add spacing for sorting arrow
         c_Header += "   ";
         c_Retval = c_Header;
      }
      else if (os32_Role == ms32_USER_ROLE_TOOL_TIP_HEADING)
      {
         switch (e_Col)
         {
         case eRTR:
            c_Retval = C_GtGetText::h_GetText("Remote Transmission Request");
            break;
         case eNAME:
            c_Retval = C_GtGetText::h_GetText("Name");
            break;
         case eID:
            c_Retval = C_GtGetText::h_GetText("Identifier");
            break;
         case eDLC:
            c_Retval = C_GtGetText::h_GetText("Data Length Code");
            break;
         case eDATA:
            c_Retval = C_GtGetText::h_GetText("Data");
            break;
         case eCYCLIC_TRIGGER:
            c_Retval = C_GtGetText::h_GetText("Cyclic Trigger");
            break;
         case eCYCLIC_TIME:
            c_Retval = C_GtGetText::h_GetText("Cycle Time");
            break;
         case eMANUAL_TRIGGER:
            c_Retval = C_GtGetText::h_GetText("Manual Trigger");
            break;
         case eXTD:
            c_Retval = C_GtGetText::h_GetText("Extended");
            break;
         case eKEY:
            c_Retval = C_GtGetText::h_GetText("Key");
            break;
         case eAUTO_SUPPORT:
            c_Retval = C_GtGetText::h_GetText("Auto ECeS/ECoS Protocol Support");
            break;
         default:
            tgl_assert(false);
            break;
         }
      }
      else if (os32_Role == static_cast<int32_t>(Qt::TextAlignmentRole))
      {
         c_Retval = static_cast<QVariant>(Qt::AlignLeft | Qt::AlignVCenter);
      }
      else if (os32_Role == ms32_USER_ROLE_TOOL_TIP_CONTENT)
      {
         QString c_Header;
         switch (e_Col)
         {
         case eRTR:
            c_Header = C_GtGetText::h_GetText("CAN message sent as remote transmission frame.");
            break;
         case eNAME:
            c_Header = C_GtGetText::h_GetText("Symbolic name of the CAN message.");
            break;
         case eID:
            c_Header = C_GtGetText::h_GetText("CAN identifier of the message.");
            break;
         case eDLC:
            c_Header = C_GtGetText::h_GetText("Number of data bytes in decimal representation.");
            break;
         case eDATA:
            c_Header = C_GtGetText::h_GetText("CAN message data in bytes.");
            break;
         case eCYCLIC_TRIGGER:
            c_Header = C_GtGetText::h_GetText("Transmit this CAN message cyclically.");
            break;
         case eCYCLIC_TIME:
            c_Header = C_GtGetText::h_GetText("CAN message cycle time in ms.");
            break;
         case eMANUAL_TRIGGER:
            c_Header = C_GtGetText::h_GetText("Trigger the transmission of this CAN message manually.");
            break;
         case eXTD:
            c_Header = C_GtGetText::h_GetText("CAN message ID uses extended format.");
            break;
         case eKEY:
            c_Header = C_GtGetText::h_GetText("Configure key for triggering transmission of message.");
            break;
         case eAUTO_SUPPORT:
            c_Header = C_GtGetText::h_GetText("ECeS: When sending automatic calculation of the CRC and incrementing of "
                                              "the block counter is applied.\n"
                                              "ECoS: When sending the inverted second frame will be sent automatically "
                                              "immediately after the first frame gets sent.");
            break;
         default:
            tgl_assert(false);
            break;
         }
         c_Retval = c_Header;
      }
      else
      {
         //No data in default case
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get table column count

   \param[in]  orc_Parent  Parent

   \return
   Column count
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CamGenTableModel::columnCount(const QModelIndex & orc_Parent) const
{
   int32_t s32_Retval = 0;

   if (!orc_Parent.isValid())
   {
      //For table parent should always be invalid
      s32_Retval = 11;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get data at index

   \param[in]  orc_Index   Index
   \param[in]  os32_Role    Data role

   \return
   Data
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_CamGenTableModel::data(const QModelIndex & orc_Index, const int32_t os32_Role) const
{
   QVariant c_Retval;

   if ((orc_Index.isValid() == true) && (orc_Index.row() >= 0))
   {
      const uint32_t u32_Index = static_cast<uint32_t>(orc_Index.row());
      const C_CamProMessageData * const pc_Message = C_CamProHandler::h_GetInstance()->GetMessageConst(u32_Index);
      if (pc_Message != NULL)
      {
         const C_CamGenTableModel::E_Columns e_Col = h_ColumnToEnum(orc_Index.column());
         if (((os32_Role == static_cast<int32_t>(Qt::DisplayRole)) ||
              (os32_Role == static_cast<int32_t>(Qt::EditRole))) ||
             (os32_Role == ms32_USER_ROLE_SORT))
         {
            switch (e_Col)
            {
            case eNAME:
               c_Retval = C_CamProHandler::h_GetCompleteMessageName(*pc_Message);
               break;
            case eID:
               if (os32_Role == ms32_USER_ROLE_SORT)
               {
                  //Use simple unit value for sorting (better than string)
                  c_Retval = static_cast<uint64_t>(pc_Message->u32_Id);
               }
               else
               {
                  c_Retval = mh_HandleHexValue(static_cast<uint64_t>(pc_Message->u32_Id), os32_Role);
               }
               break;
            case eDLC:
               c_Retval = static_cast<uint64_t>(pc_Message->u16_Dlc);
               break;
            case eDATA:
               //Don't set any data as this column is handled differently
               if (os32_Role == ms32_USER_ROLE_SORT)
               {
                  //For sorting!
                  uint64_t u64_Result = 0ULL;
                  //Right align all filled bytes
                  for (uint16_t u16_ItByte = 0U; u16_ItByte < pc_Message->u16_Dlc; ++u16_ItByte)
                  {
                     u64_Result += static_cast<uint64_t>(pc_Message->c_Bytes[static_cast<uint32_t>(u16_ItByte)]) <<
                                   (56U - (u16_ItByte * 8U));
                  }
                  c_Retval = u64_Result;
               }
               break;
            case eCYCLIC_TIME:
               c_Retval = static_cast<uint64_t>(pc_Message->u32_CyclicTriggerTime);
               break;
            case eMANUAL_TRIGGER:
               c_Retval = C_GtGetText::h_GetText("Send now");
               break;
            case eKEY:
               if (pc_Message->c_Key.IsEmpty())
               {
                  c_Retval = C_GtGetText::h_GetText("<none>");
               }
               else
               {
                  QString c_Offset;
                  if (pc_Message->u32_KeyPressOffset > 0UL)
                  {
                     c_Offset = static_cast<QString>(" (+%1ms)").arg(pc_Message->u32_KeyPressOffset);
                  }
                  c_Retval = static_cast<QString>("%1%2").arg(pc_Message->c_Key.c_str()).arg(c_Offset);
               }
               break;
            case eRTR:
               //Different handling for booleans
               if (os32_Role == ms32_USER_ROLE_SORT)
               {
                  //For sorting!
                  c_Retval = pc_Message->GetRtr();
               }
               break;
            case eCYCLIC_TRIGGER:
               //Different handling for booleans
               if (os32_Role == ms32_USER_ROLE_SORT)
               {
                  //For sorting!
                  c_Retval = pc_Message->q_DoCyclicTrigger;
               }
               break;
            case eXTD:
               //Different handling for booleans
               if (os32_Role == ms32_USER_ROLE_SORT)
               {
                  //For sorting!
                  c_Retval = pc_Message->GetExtended();
               }
               break;
            case eAUTO_SUPPORT:
               //Different handling for booleans
               if (os32_Role == ms32_USER_ROLE_SORT)
               {
                  //For sorting!
                  c_Retval = pc_Message->q_SetAutoSupportMode;
               }
               break;
            default:
               tgl_assert(false);
               break;
            }
         }
         else if (os32_Role == static_cast<int32_t>(Qt::ForegroundRole))
         {
            if (this->data(orc_Index, ms32_USER_ROLE_INTERACTION_IS_LINK).toBool() == true)
            {
               c_Retval = mc_STYLE_GUIDE_COLOR_4;
            }
            else
            {
               if (this->flags(orc_Index).testFlag(Qt::ItemIsEditable))
               {
                  c_Retval = mc_STYLE_GUIDE_COLOR_1;
               }
               else
               {
                  c_Retval = mc_STYLE_GUIDE_COLOR_8;
               }
            }
         }
         else if (os32_Role == ms32_USER_ROLE_MARKER)
         {
            if (e_Col == eDATA)
            {
               QBitArray c_Array;
               //No markings
               c_Array.resize(static_cast<int32_t>(pc_Message->u16_Dlc));
               c_Retval = c_Array;
            }
         }
         else if (os32_Role == ms32_USER_ROLE_MARKER_TEXT)
         {
            if (e_Col == eDATA)
            {
               if ((pc_Message->q_SetAutoSupportMode != true) ||
                   (C_CamGenTableModel::m_GetCurrentMessageProtocolType(u32_Index) != C_OscCanProtocol::eECES))
               {
                  QString c_Tmp;
                  for (uint16_t u16_It = 0U; u16_It < pc_Message->u16_Dlc; ++u16_It)
                  {
                     if (c_Tmp.isEmpty() == false)
                     {
                        c_Tmp += " ";
                     }
                     c_Tmp += static_cast<QString>("%1").arg(pc_Message->c_Bytes[u16_It], 2, 16, QChar('0')).toUpper();
                  }
                  c_Retval = c_Tmp;
               }
               else
               {
                  c_Retval = UpdateDataForAutoProtocol(orc_Index);
               }
            }
         }
         else if (os32_Role == ms32_USER_ROLE_MARKER_TRANSPARENCY)
         {
            if (e_Col == eDATA)
            {
               QByteArray c_Array;
               c_Array.resize(static_cast<int32_t>(pc_Message->u16_Dlc));
               //No transparency
               c_Array.fill(static_cast<char_t>(0));
               c_Retval = c_Array;
            }
         }
         else if (os32_Role == ms32_USER_ROLE_INTERACTION_ELEMENT_TYPE)
         {
            switch (e_Col) //lint !e788 //not all columns explicitly handled
            {
            case eNAME:
            case eID:
            case eCYCLIC_TIME:
               c_Retval = static_cast<int32_t>(eURIEL_LINE_EDIT);
               break;
            case eDLC:
               c_Retval = static_cast<int32_t>(eURIEL_COMBO_BOX);
               break;
            default:
               c_Retval = static_cast<int32_t>(eURIEL_NONE);
               break;
            }
         }
         else if ((os32_Role == ms32_USER_ROLE_INTERACTION_COMBO_BOX_STRINGS_LIST) ||
                  (os32_Role == ms32_USER_ROLE_INTERACTION_COMBO_BOX_VALUES_LIST))
         {
            QStringList c_Tmp;
            if (e_Col == eDLC)
            {
               c_Tmp.append("0");
               c_Tmp.append("1");
               c_Tmp.append("2");
               c_Tmp.append("3");
               c_Tmp.append("4");
               c_Tmp.append("5");
               c_Tmp.append("6");
               c_Tmp.append("7");
               c_Tmp.append("8");
               c_Retval = c_Tmp;
            }
            else
            {
               c_Retval = c_Tmp;
            }
         }
         else if (os32_Role == ms32_USER_ROLE_INTERACTION_IS_LINK)
         {
            switch (e_Col) //lint !e788 //not all columns explicitly handled
            {
            case eKEY:
            case eMANUAL_TRIGGER:
               c_Retval = true;
               break;
            default:
               c_Retval = false;
               break;
            }
         }
         else if (os32_Role == ms32_USER_ROLE_INTERACTION_USE_MIN_VALUE)
         {
            switch (e_Col) //lint !e788 //not all columns explicitly handled
            {
            case eID:
            case eDLC:
            case eCYCLIC_TIME:
               c_Retval = true;
               break;
            default:
               c_Retval = false;
               break;
            }
         }
         else if (os32_Role == ms32_USER_ROLE_INTERACTION_MINIMUM_VALUE)
         {
            switch (e_Col) //lint !e788 //not all columns explicitly handled
            {
            case eID:
               c_Retval = C_Uti::h_GetValueAsHex(0ULL);
               break;
            case eDLC:
               c_Retval = 0;
               break;
            case eCYCLIC_TIME:
               c_Retval = 1;
               break;
            default:
               //None
               break;
            }
         }
         else if (os32_Role == ms32_USER_ROLE_INTERACTION_USE_MAX_VALUE)
         {
            switch (e_Col) //lint !e788 //not all columns explicitly handled
            {
            case eID:
            case eDLC:
            case eCYCLIC_TIME:
               c_Retval = true;
               break;
            default:
               c_Retval = false;
               break;
            }
         }
         else if (os32_Role == ms32_USER_ROLE_INTERACTION_MAXIMUM_VALUE)
         {
            switch (e_Col) //lint !e788 //not all columns explicitly handled
            {
            case eID:
               if (pc_Message->GetExtended())
               {
                  c_Retval = C_Uti::h_GetValueAsHex(0x1FFFFFFFULL);
               }
               else
               {
                  c_Retval = C_Uti::h_GetValueAsHex(0x7FFULL);
               }
               break;
            case eDLC:
               c_Retval = 8;
               break;
            case eCYCLIC_TIME:
               c_Retval = 50000;
               break;
            default:
               //None
               break;
            }
         }
         else if (os32_Role == static_cast<int32_t>(Qt::CheckStateRole))
         {
            switch (e_Col)
            {
            case eKEY:
            case eNAME:
            case eID:
            case eDLC:
            case eDATA:
            case eCYCLIC_TIME:
            case eMANUAL_TRIGGER:
               //Booleans only
               break;
            case eCYCLIC_TRIGGER:
               c_Retval = mh_GetBoolAsCheckStateVariant(pc_Message->q_DoCyclicTrigger);
               break;
            case eXTD:
               c_Retval = mh_GetBoolAsCheckStateVariant(pc_Message->GetExtended());
               break;
            case eRTR:
               c_Retval = mh_GetBoolAsCheckStateVariant(pc_Message->GetRtr());
               break;
            case eAUTO_SUPPORT:
               c_Retval = mh_GetBoolAsCheckStateVariant(pc_Message->q_SetAutoSupportMode);
               break;
            default:
               tgl_assert(false);
               break;
            }
         }
         else if (os32_Role == static_cast<int32_t>(Qt::FontRole))
         {
            QFont c_Font = C_Uti::h_GetFontPixel(mc_STYLE_GUIDE_FONT_REGULAR_12);
            if (this->data(orc_Index, ms32_USER_ROLE_INTERACTION_IS_LINK).toBool() == true)
            {
               //Special link handling
               c_Font.setUnderline(true);
            }
            c_Retval = c_Font;
         }
         else
         {
            //Not necessary yet
         }
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set data at index

   \param[in]  orc_Index   Index
   \param[in]  orc_Value   New data
   \param[in]  os32_Role    Data role

   \return
   true  success
   false failure
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_CamGenTableModel::setData(const QModelIndex & orc_Index, const QVariant & orc_Value, const int32_t os32_Role)
{
   bool q_Retval = false;

   if (data(orc_Index, os32_Role) != orc_Value)
   {
      if ((orc_Index.isValid() == true) && (orc_Index.row() >= 0))
      {
         bool q_UpdateCyclicMessage = false;
         const uint32_t u32_Index = static_cast<uint32_t>(orc_Index.row());
         const C_CamGenTableModel::E_Columns e_Col = h_ColumnToEnum(orc_Index.column());
         if (os32_Role == static_cast<int32_t>(Qt::EditRole))
         {
            bool q_Continue = false;
            C_CamProMessageData::E_GenericUint32DataSelector e_Selector;
            switch (e_Col)
            {
            case eNAME:
               tgl_assert(C_CamProHandler::h_GetInstance()->SetMessageName(u32_Index,
                                                                           orc_Value.toString()) == C_NO_ERR);
               q_Retval = true;
               break;
            case eID:
               q_UpdateCyclicMessage = true;
               e_Selector = C_CamProMessageData::eGUIDS_ID;
               q_Continue = true;
               q_Retval = true;
               break;
            case eDLC:
               q_UpdateCyclicMessage = true;
               e_Selector = C_CamProMessageData::eGUIDS_DLC;
               q_Continue = true;
               q_Retval = true;
               break;
            case eDATA:
               //Unknown yet
               break;
            case eCYCLIC_TIME:
               q_UpdateCyclicMessage = true;
               e_Selector = C_CamProMessageData::eGUIDS_CYCLIC_TIME;
               q_Continue = true;
               q_Retval = true;
               break;
            case eMANUAL_TRIGGER:
               //Should not happen
               break;
            case eKEY:
               //Should not happen
               break;
            case eRTR:
            case eCYCLIC_TRIGGER:
            case eXTD:
               //Different handling for booleans
               break;
            case eAUTO_SUPPORT:
               break;
            default:
               tgl_assert(false);
               break;
            }
            if (q_Continue == true)
            {
               const uint32_t u32_Value = static_cast<uint32_t>(orc_Value.toULongLong());
               //Deactivate message as it is sent now
               if (q_UpdateCyclicMessage == true)
               {
                  m_CheckAndHandleRegisterCyclicMessage(u32_Index, false);
               }
               //lint -e644 that's what the q_Continue is for
               tgl_assert(C_CamProHandler::h_GetInstance()->SetMessageUint32Value(u32_Index, e_Selector,
                                                                                  u32_Value) == C_NO_ERR);
               q_Retval = true;
               //Trigger sending of updated message
               if (q_UpdateCyclicMessage == true)
               {
                  m_CheckAndHandleRegisterCyclicMessage(u32_Index, true);
               }
               //Special for DLC: update Data column and notify signal table!
               if (e_Col == eDLC)
               {
                  const int32_t s32_Col = C_CamGenTableModel::h_EnumToColumn(eDATA);
                  const QModelIndex c_Temp = orc_Index.sibling(orc_Index.row(), s32_Col);

                  Q_EMIT (this->SigUpdateMessageDlc(u32_Index));
                  //lint -e{1793} Qt example
                  Q_EMIT (this->dataChanged(c_Temp, c_Temp, QVector<int32_t>() << os32_Role));
               }
            }
         }
         else if (os32_Role == static_cast<int32_t>(Qt::CheckStateRole))
         {
            if (orc_Value.type() == QVariant::Int)
            {
               bool q_Continue = false;
               C_CamProMessageData::E_GenericBoolDataSelector e_Selector;
               switch (e_Col)
               {
               case eKEY:
               case eNAME:
               case eID:
               case eDLC:
               case eDATA:
               case eCYCLIC_TIME:
               case eMANUAL_TRIGGER:
                  //Booleans only
                  break;
               case eRTR:
                  q_UpdateCyclicMessage = true;
                  e_Selector = C_CamProMessageData::eGBODS_RTR;
                  q_Continue = true;
                  break;
               case eCYCLIC_TRIGGER:
                  e_Selector = C_CamProMessageData::eGBODS_DO_CYCLIC;
                  q_Continue = true;
                  break;
               case eXTD:
                  q_UpdateCyclicMessage = true;
                  e_Selector = C_CamProMessageData::eGBODS_EXTENDED;
                  q_Continue = true;
                  break;
               case eAUTO_SUPPORT:
                  e_Selector = C_CamProMessageData::eGBODS_AUTO_SUPPORT;
                  q_Continue = true;
                  break;
               default:
                  tgl_assert(false);
                  break;
               }
               if (q_Continue == true)
               {
                  const bool q_Val = mh_GetCheckStateVariantAsBool(orc_Value);
                  //Deactivate message as it is sent now
                  if (q_UpdateCyclicMessage == true)
                  {
                     m_CheckAndHandleRegisterCyclicMessage(u32_Index, false);
                  }
                  //lint -e644 that's what the q_Continue is for
                  tgl_assert(C_CamProHandler::h_GetInstance()->SetMessageBoolValue(u32_Index, e_Selector,
                                                                                   q_Val) == C_NO_ERR);
                  //Special handling for XTD
                  if ((e_Col == eXTD) && (q_Val == false))
                  {
                     m_SpecialXtdFlagSetHandling(orc_Index.row(), u32_Index, os32_Role);
                  }
                  //Trigger sending of updated message
                  if (q_UpdateCyclicMessage == true)
                  {
                     m_CheckAndHandleRegisterCyclicMessage(u32_Index, true);
                  }
                  else
                  {
                     //Special for cyclic trigger: change the current message as necessary (no two signals necessary)
                     if ((e_Col == eCYCLIC_TRIGGER) &&
                         (C_CamProHandler::h_GetInstance()->GetCyclicMessageTransmitActive() == true))
                     {
                        Q_EMIT (this->SigRegisterCyclicMessage(u32_Index, q_Val));
                     }
                  }
                  if (e_Col == eAUTO_SUPPORT)
                  {
                     this->UpdateMessageData(u32_Index);
                     this->UpdateAutoProtocolCellData(u32_Index);
                     Q_EMIT (this->SigAutoProtocolSupport(u32_Index, q_Val));
                  }
                  q_Retval = true;
                  //Signal check change
                  Q_EMIT (this->SigItemCheck(orc_Index));
               }
            }
         }
         else
         {
            //Not necessary yet
         }
         if (q_Retval == true)
         {
            //lint -e{1793} Qt example
            Q_EMIT (this->dataChanged(orc_Index, orc_Index, QVector<int32_t>() << os32_Role));
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get flags for item

   \param[in]  orc_Index   Item

   \return
   Flags for item
*/
//----------------------------------------------------------------------------------------------------------------------
Qt::ItemFlags C_CamGenTableModel::flags(const QModelIndex & orc_Index) const
{
   Qt::ItemFlags c_Retval = Qt::ItemIsSelectable;
   if (orc_Index.isValid() == true)
   {
      const C_CamGenTableModel::E_Columns e_Col = h_ColumnToEnum(orc_Index.column());
      switch (e_Col)
      {
      case eNAME:
      case eID:
      case eDLC:
         if (orc_Index.row() >= 0)
         {
            const C_CamProMessageData * const pc_Message = C_CamProHandler::h_GetInstance()->GetMessageConst(
               static_cast<uint32_t>(orc_Index.row()));
            if ((pc_Message != NULL) && (pc_Message->c_DataBaseFilePath.IsEmpty() == true))
            {
               c_Retval = c_Retval | Qt::ItemIsEditable | Qt::ItemIsEnabled;
            }
            else
            {
               c_Retval = c_Retval | Qt::ItemIsEnabled;
            }
         }
         break;
      case eKEY:
      case eCYCLIC_TIME:
      case eMANUAL_TRIGGER:
         c_Retval = c_Retval | Qt::ItemIsEditable | Qt::ItemIsEnabled;
         break;
      case eCYCLIC_TRIGGER:
         c_Retval = c_Retval | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
         break;
      case eXTD:
      case eRTR:
         if (orc_Index.row() >= 0)
         {
            const C_CamProMessageData * const pc_Message = C_CamProHandler::h_GetInstance()->GetMessageConst(
               static_cast<uint32_t>(orc_Index.row()));
            if ((pc_Message != NULL) && (pc_Message->c_DataBaseFilePath.IsEmpty() == true))
            {
               c_Retval = c_Retval | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
            }
         }
         break;
      case eDATA:
         c_Retval = c_Retval | Qt::ItemIsEnabled;
         break;
      case eAUTO_SUPPORT:
         if ((C_CamGenTableModel::m_GetCurrentMessageProtocolType(static_cast<uint32_t>(orc_Index.row())) ==
              C_OscCanProtocol::eECES) ||
             (C_CamGenTableModel::m_GetCurrentMessageProtocolType(static_cast<uint32_t>(orc_Index.row())) ==
              C_OscCanProtocol::eCAN_OPEN_SAFETY))
         {
            c_Retval = c_Retval | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
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
/*! \brief  Handle copy items action

   \param[in]  orc_SelectedIndices  Selected row indices (Expected: unique)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableModel::CopySelectedItems(const std::vector<uint32_t> & orc_SelectedIndices) const
{
   std::vector<C_CamProMessageData> c_Messages;
   for (uint32_t u32_It = 0UL; u32_It < orc_SelectedIndices.size(); ++u32_It)
   {
      const C_CamProMessageData * const pc_Message = C_CamProHandler::h_GetInstance()->GetMessageConst(
         orc_SelectedIndices[u32_It]);
      if (pc_Message != NULL)
      {
         c_Messages.push_back(*pc_Message);
      }
   }
   C_CamProClipBoardHelper::h_SaveMessages(c_Messages);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Column to enum conversion

   \param[in]  os32_Column    Column

   \return
   Enum value
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamGenTableModel::E_Columns C_CamGenTableModel::h_ColumnToEnum(const int32_t os32_Column)
{
   C_CamGenTableModel::E_Columns e_Retval;
   switch (os32_Column)
   {
   case 0:
      e_Retval = eNAME;
      break;
   case 1:
      e_Retval = eID;
      break;
   case 2:
      e_Retval = eXTD;
      break;
   case 3:
      e_Retval = eRTR;
      break;
   case 4:
      e_Retval = eDLC;
      break;
   case 5:
      e_Retval = eDATA;
      break;
   case 6:
      e_Retval = eCYCLIC_TRIGGER;
      break;
   case 7:
      e_Retval = eCYCLIC_TIME;
      break;
   case 8:
      e_Retval = eAUTO_SUPPORT;
      break;
   case 9:
      e_Retval = eKEY;
      break;
   case 10:
      e_Retval = eMANUAL_TRIGGER;
      break;
   default:
      e_Retval = eNAME;
      break;
   }
   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Enum to column conversion

   \param[in]  oe_Value    Enum value

   \return
   Column
   -1 Error
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CamGenTableModel::h_EnumToColumn(const C_CamGenTableModel::E_Columns oe_Value)
{
   int32_t s32_Retval = -1;

   switch (oe_Value)
   {
   case eNAME:
      s32_Retval = 0;
      break;
   case eID:
      s32_Retval = 1;
      break;
   case eXTD:
      s32_Retval = 2;
      break;
   case eRTR:
      s32_Retval = 3;
      break;
   case eDLC:
      s32_Retval = 4;
      break;
   case eDATA:
      s32_Retval = 5;
      break;
   case eCYCLIC_TRIGGER:
      s32_Retval = 6;
      break;
   case eCYCLIC_TIME:
      s32_Retval = 7;
      break;
   case eAUTO_SUPPORT:
      s32_Retval = 8;
      break;
   case eKEY:
      s32_Retval = 9;
      break;
   case eMANUAL_TRIGGER:
      s32_Retval = 10;
      break;
   default:
      tgl_assert(false);
      break;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle add specific new items action

   \param[in]  orc_SelectedIndex    Selected indices
   \param[in]  orc_Messages         Messages to add

   \return
   Index of new item
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<uint32_t> C_CamGenTableModel::AddSpecificNewItems(const std::vector<uint32_t> & orc_SelectedIndex,
                                                              const std::vector<C_CamProMessageData> & orc_Messages)
{
   std::vector<uint32_t> c_Retval;
   c_Retval = m_AddNewMessages(m_GetLastSelectedIndex(orc_SelectedIndex), orc_Messages);
   Q_EMIT (this->SigItemCountChanged(this->m_GetSizeItems()));
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update data feild for ECeS Messages when Auto Protocol is selected

   \param[in]       orc_Index     Index at which the changed to be done

   \return
   QString String data of message data field
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_CamGenTableModel::UpdateDataForAutoProtocol(const QModelIndex & orc_Index) const
{
   QString c_Tmp;

   if ((orc_Index.isValid() == true) && (orc_Index.row() >= 0))
   {
      const uint32_t u32_Index = static_cast<uint32_t>(orc_Index.row());
      const C_CamProMessageData * const pc_Message = C_CamProHandler::h_GetInstance()->GetMessageConst(u32_Index);

      for (uint16_t u16_It = 0U; u16_It < pc_Message->u16_Dlc; ++u16_It)
      {
         if (c_Tmp.isEmpty() == false)
         {
            c_Tmp += " ";
         }
         if (((pc_Message->q_SetAutoSupportMode == true) &&
              (C_CamGenTableModel::m_GetCurrentMessageProtocolType(u32_Index) == C_OscCanProtocol::eECES)) &&
             ((u16_It == 6) || (u16_It == 7)))
         {
            c_Tmp += "XX";
         }
         else
         {
            c_Tmp += static_cast<QString>("%1").arg(pc_Message->c_Bytes[u16_It], 2, 16, QChar('0')).toUpper();
         }
      }
   }
   return c_Tmp;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle add new item action

   \param[in]  ou32_SelectedIndex   Index to insert item at

   \return
   Index of new item
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_CamGenTableModel::m_AddNewItem(const uint32_t ou32_SelectedIndex)
{
   uint32_t u32_Retval;

   std::vector<uint32_t> c_Items;
   std::vector<C_CamProMessageData> c_Messages;
   C_CamProMessageData c_NewMessage;

   //Initial name
   c_NewMessage.c_Name = "New message";
   //initial DLC
   c_NewMessage.u16_Dlc = 8U;
   c_Messages.push_back(c_NewMessage);
   c_Items = m_AddNewMessages(ou32_SelectedIndex, c_Messages);
   if (c_Items.size() > 0UL)
   {
      u32_Retval = c_Items[0UL];
   }
   else
   {
      u32_Retval = 0UL;
   }
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle paste items action

   \param[in]  ou32_SelectedIndex   Index to insert item at

   \return
   Indices of new items
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<uint32_t> C_CamGenTableModel::m_PasteItems(const uint32_t ou32_SelectedIndex)
{
   std::vector<uint32_t> c_Retval;
   std::vector<C_CamProMessageData> c_Messages;
   if (C_CamProClipBoardHelper::h_LoadMessages(c_Messages) == C_NO_ERR)
   {
      c_Retval = m_AddNewMessages(ou32_SelectedIndex, c_Messages);
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get size of item container

   \return
   Size of item container
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_CamGenTableModel::m_GetSizeItems(void) const
{
   return C_CamProHandler::h_GetInstance()->GetMessages().size();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Delete specific item

   Warning: not expected to fail

   \param[in]  ou32_Index  Index to delete
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableModel::m_DeleteItem(const uint32_t ou32_Index)
{
   tgl_assert(C_CamProHandler::h_GetInstance()->DeleteMessage(ou32_Index) == C_NO_ERR);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Move specific item

   For implementation: First delete ou32_SourceIndex then insert ou32_TargetIndex
   Warning: not expected to fail

   \param[in]  ou32_SourceIndex  Source index
   \param[in]  ou32_TargetIndex  Target index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableModel::m_MoveItem(const uint32_t ou32_SourceIndex, const uint32_t ou32_TargetIndex)
{
   tgl_assert(C_CamProHandler::h_GetInstance()->MoveMessage(ou32_SourceIndex, ou32_TargetIndex) == C_NO_ERR);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add specified new messages

   \param[in]  ou32_SelectedIndex   Selected row index
   \param[in]  orc_Data             New messages

   \return
   Indices of new items
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<uint32_t> C_CamGenTableModel::m_AddNewMessages(const uint32_t ou32_SelectedIndex,
                                                           const std::vector<C_CamProMessageData> & orc_Data)
{
   uint32_t u32_Index;

   std::vector<uint32_t> c_Retval;

   if (ou32_SelectedIndex < C_CamProHandler::h_GetInstance()->GetMessages().size())
   {
      u32_Index = ou32_SelectedIndex + 1UL;
   }
   else
   {
      u32_Index = C_CamProHandler::h_GetInstance()->GetMessages().size();
   }

   if (orc_Data.size() > 0UL)
   {
      this->beginInsertRows(QModelIndex(), u32_Index, static_cast<int32_t>((u32_Index + orc_Data.size()) - 1UL));
      c_Retval.reserve(orc_Data.size());
      for (uint32_t u32_It = 0UL; u32_It < orc_Data.size(); ++u32_It)
      {
         C_CamProHandler::h_GetInstance()->InsertMessage(u32_Index + u32_It, orc_Data[u32_It], true);
         c_Retval.push_back(u32_Index + u32_It);
      }
      this->endInsertRows();
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle cyclic change request

   Add check if necessary

   \param[in]  ou32_MessageIndex    Message index
   \param[in]  oq_Active            Flag if cyclic message is active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableModel::m_CheckAndHandleRegisterCyclicMessage(const uint32_t ou32_MessageIndex, const bool oq_Active)
{
   const C_CamProMessageData * const pc_Message = C_CamProHandler::h_GetInstance()->GetMessageConst(ou32_MessageIndex);

   //Only send this signal if the message is currently cyclic
   if ((pc_Message != NULL) && (pc_Message->q_DoCyclicTrigger == true) &&
       ((C_CamProHandler::h_GetInstance()->GetCyclicMessageTransmitActive() == true)))
   {
      Q_EMIT (this->SigRegisterCyclicMessage(ou32_MessageIndex, oq_Active));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Special xtd flag set handling

   \param[in]  os32_Row     Row
   \param[in]  ou32_Index  Index
   \param[in]  os32_Role    Role
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableModel::m_SpecialXtdFlagSetHandling(const int32_t os32_Row, const uint32_t ou32_Index,
                                                     const int32_t os32_Role)
{
   const int32_t s32_Col = C_CamGenTableModel::h_EnumToColumn(eID);
   const uint64_t u64_MessageId =
      this->data(this->index(os32_Row, s32_Col), ms32_USER_ROLE_SORT).toULongLong();

   if (u64_MessageId > 0x7FFULL)
   {
      constexpr C_CamProMessageData::E_GenericUint32DataSelector e_SELECTOR_ID =
         C_CamProMessageData::eGUIDS_ID;
      constexpr uint32_t u32_NEW_VAL = 0x0UL;
      tgl_assert(C_CamProHandler::h_GetInstance()->SetMessageUint32Value(ou32_Index, e_SELECTOR_ID,
                                                                         u32_NEW_VAL) == C_NO_ERR);
      //Message
      Q_EMIT (this->SigReport(stw::opensyde_gui_elements::C_OgeWiCustomMessage::eWARNING,
                              C_GtGetText::h_GetText("Check Message ID"),
                              C_GtGetText::h_GetText(
                                 "XTD flag is disabled, message ID is invalid and is reset to \"0\"")));

      //lint -e{1793} Qt example
      Q_EMIT (this->dataChanged(this->index(os32_Row, s32_Col),
                                this->index(os32_Row, s32_Col),
                                QVector<int32_t>() << os32_Role));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  getting message protocol type

   \param[in]  ou32_MessageIndex    Message index

   \return
   return type of protocol for current message

   \retval   eLAYER2,           Data pool communication protocol OSI layer 2
   \retval   eCAN_OPEN_SAFETY,  Data pool communication protocol CAN open safety (safety protocol)
   \retval   eECES,             Data pool communication protocol ECeS (safety protocol)
   \retval   eJ1939,            Data pool communication protocol J1939
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscCanProtocol::E_Type C_CamGenTableModel::m_GetCurrentMessageProtocolType(const uint32_t ou32_MessageIndex) const
{
   const C_CamProMessageData * const pc_Message = C_CamProHandler::h_GetInstance()->GetMessageConst(
      ou32_MessageIndex);

   C_OscCanProtocol::E_Type e_ProtocolType = C_OscCanProtocol::eCAN_OPEN;
   if (pc_Message != NULL)
   {
      if (C_CamDbHandler::h_GetInstance()->FindOsyMessage(pc_Message->c_DataBaseFilePath.c_str(),
                                                          pc_Message->c_Name.c_str()) == C_NO_ERR)
      {
         if (C_CamDbHandler::h_GetInstance()->GetOscMessage(pc_Message->c_DataBaseFilePath.c_str(),
                                                            pc_Message->c_Name.c_str(),
                                                            pc_Message->q_ContainsValidHash,
                                                            pc_Message->u32_Hash,
                                                            &e_ProtocolType) == NULL)
         {
            // Db engine is not ready yet to get the current message protocol
            // It is totally fine to be failed at this point, as still the DB file is in loading process
         }
      }
   }
   return e_ProtocolType;
}
