//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       short description (implementation)

   detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "constants.h"
#include "C_GtGetText.h"
#include "C_PuiSdHandler.h"
#include "C_SdNdeSfoResetMessageTableModel.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const sintn C_SdNdeSfoResetMessageTableModel::mhsn_INDEX_STANDARD = 0;
const sintn C_SdNdeSfoResetMessageTableModel::mhsn_INDEX_EXTENDED = 1;

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
C_SdNdeSfoResetMessageTableModel::C_SdNdeSfoResetMessageTableModel(QObject * const opc_Parent) :
   QAbstractTableModel(opc_Parent),
   mu32_NodeIndex(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Specify associated node

   \param[in] ou32_NodeIndex Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeSfoResetMessageTableModel::SetIndex(const uint32 ou32_NodeIndex)
{
   beginResetModel();
   this->mu32_NodeIndex = ou32_NodeIndex;
   endResetModel();
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
QVariant C_SdNdeSfoResetMessageTableModel::headerData(const sintn osn_Section, const Qt::Orientation oe_Orientation,
                                                      const sintn osn_Role) const
{
   QVariant c_Retval = QAbstractTableModel::headerData(osn_Section, oe_Orientation, osn_Role);

   if (oe_Orientation == Qt::Orientation::Horizontal)
   {
      const C_SdNdeSfoResetMessageTableModel::E_Columns e_Col = C_SdNdeSfoResetMessageTableModel::h_ColumnToEnum(
         osn_Section);
      if (osn_Role == static_cast<sintn>(Qt::DisplayRole))
      {
         switch (e_Col)
         {
         case eID:
            c_Retval = C_GtGetText::h_GetText("CAN ID");
            break;
         case eEXTENDED_FORMAT:
            c_Retval = C_GtGetText::h_GetText("Type");
            break;
         case eDLC:
            c_Retval = C_GtGetText::h_GetText("DLC");
            break;
         case eBYTE1:
            c_Retval = C_GtGetText::h_GetText("Byte 1");
            break;
         case eBYTE2:
            c_Retval = C_GtGetText::h_GetText("Byte 2");
            break;
         case eBYTE3:
            c_Retval = C_GtGetText::h_GetText("Byte 3");
            break;
         case eBYTE4:
            c_Retval = C_GtGetText::h_GetText("Byte 4");
            break;
         case eBYTE5:
            c_Retval = C_GtGetText::h_GetText("Byte 5");
            break;
         case eBYTE6:
            c_Retval = C_GtGetText::h_GetText("Byte 6");
            break;
         case eBYTE7:
            c_Retval = C_GtGetText::h_GetText("Byte 7");
            break;
         case eBYTE8:
            c_Retval = C_GtGetText::h_GetText("Byte 8");
            break;
         default:
            break;
         }
      }
      else if (osn_Role == static_cast<sintn>(Qt::TextAlignmentRole))
      {
         c_Retval = QVariant(static_cast<sintn>(Qt::AlignCenter));
      }
      else if (osn_Role == static_cast<sintn>(Qt::ForegroundRole))
      {
         const QColor c_Active = mc_STYLE_GUIDE_COLOR_3;
         const QColor c_Disabled = mc_STYLE_GUIDE_COLOR_10;
         const C_OSCNode * const pc_OSCNode = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
         c_Retval = c_Active;
         if ((pc_OSCNode != NULL) && (pc_OSCNode->c_Properties.c_STWFlashloaderSettings.q_ResetMessageActive == false))
         {
            c_Retval = c_Disabled;
         }
      }
      else
      {
         //No data in default case
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
sintn C_SdNdeSfoResetMessageTableModel::rowCount(const QModelIndex & orc_Parent) const
{
   sintn sn_Retval = 0;

   if (!orc_Parent.isValid())
   {
      return 1;
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
sintn C_SdNdeSfoResetMessageTableModel::columnCount(const QModelIndex & orc_Parent) const
{
   stw_types::sintn sn_Retval = 0;
   if (!orc_Parent.isValid())
   {
      //For table parent should always be invalid
      sn_Retval = 11;
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
QVariant C_SdNdeSfoResetMessageTableModel::data(const QModelIndex & orc_Index, const sintn osn_Role) const
{
   QVariant c_Retval;

   if (orc_Index.isValid() == true)
   {
      const C_SdNdeSfoResetMessageTableModel::E_Columns e_Col =
         C_SdNdeSfoResetMessageTableModel::h_ColumnToEnum(orc_Index.column());
      if ((osn_Role == static_cast<sintn>(Qt::DisplayRole)) || (osn_Role == static_cast<sintn>(Qt::EditRole)))
      {
         uint32 u32_ByteIndex;
         const C_OSCNode * const pc_OSCNode = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
         if (pc_OSCNode != NULL)
         {
            switch (e_Col)
            {
            case eID:
               if (osn_Role == static_cast<sintn>(Qt::DisplayRole))
               {
                  c_Retval =
                     QString("0x%1").arg(QString::number(pc_OSCNode->c_Properties.c_STWFlashloaderSettings.
                                                         u32_ResetMessageId,
                                                         16).toUpper());
               }
               else
               {
                  c_Retval = static_cast<uintn>(pc_OSCNode->c_Properties.c_STWFlashloaderSettings.u32_ResetMessageId);
               }
               break;
            case eDLC:
               c_Retval = static_cast<sintn>(pc_OSCNode->c_Properties.c_STWFlashloaderSettings.u8_ResetMessageDlc);
               break;
            case eEXTENDED_FORMAT:
               if (osn_Role == static_cast<sintn>(Qt::DisplayRole))
               {
                  if (pc_OSCNode->c_Properties.c_STWFlashloaderSettings.q_ResetMessageExtendedId == true)
                  {
                     c_Retval = C_GtGetText::h_GetText("29 Bit");
                  }
                  else
                  {
                     c_Retval = C_GtGetText::h_GetText("11 Bit");
                  }
               }
               else
               {
                  if (pc_OSCNode->c_Properties.c_STWFlashloaderSettings.q_ResetMessageExtendedId == true)
                  {
                     c_Retval = C_SdNdeSfoResetMessageTableModel::mhsn_INDEX_EXTENDED;
                  }
                  else
                  {
                     c_Retval = C_SdNdeSfoResetMessageTableModel::mhsn_INDEX_STANDARD;
                  }
               }
               break;
            case eBYTE1:
               u32_ByteIndex = 0;
               if (pc_OSCNode->c_Properties.c_STWFlashloaderSettings.c_Data.size() > u32_ByteIndex)
               {
                  if (osn_Role == static_cast<sintn>(Qt::DisplayRole))
                  {
                     c_Retval =
                        QString("0x%1").arg(QString::number(pc_OSCNode->c_Properties.c_STWFlashloaderSettings.c_Data[
                                                               u32_ByteIndex], 16).toUpper());
                  }
                  else
                  {
                     c_Retval =
                        static_cast<sintn>(pc_OSCNode->c_Properties.c_STWFlashloaderSettings.c_Data[u32_ByteIndex]);
                  }
               }
               else
               {
                  if (osn_Role == static_cast<sintn>(Qt::DisplayRole))
                  {
                     c_Retval = "0x0";
                  }
                  else
                  {
                     c_Retval = 0;
                  }
               }
               break;
            case eBYTE2:
               u32_ByteIndex = 1;
               if (pc_OSCNode->c_Properties.c_STWFlashloaderSettings.c_Data.size() > u32_ByteIndex)
               {
                  if (osn_Role == static_cast<sintn>(Qt::DisplayRole))
                  {
                     c_Retval =
                        QString("0x%1").arg(QString::number(pc_OSCNode->c_Properties.c_STWFlashloaderSettings.c_Data[
                                                               u32_ByteIndex], 16).toUpper());
                  }
                  else
                  {
                     c_Retval =
                        static_cast<sintn>(pc_OSCNode->c_Properties.c_STWFlashloaderSettings.c_Data[u32_ByteIndex]);
                  }
               }
               else
               {
                  if (osn_Role == static_cast<sintn>(Qt::DisplayRole))
                  {
                     c_Retval = "0x0";
                  }
                  else
                  {
                     c_Retval = 0;
                  }
               }
               break;
            case eBYTE3:
               u32_ByteIndex = 2;
               if (pc_OSCNode->c_Properties.c_STWFlashloaderSettings.c_Data.size() > u32_ByteIndex)
               {
                  if (osn_Role == static_cast<sintn>(Qt::DisplayRole))
                  {
                     c_Retval =
                        QString("0x%1").arg(QString::number(pc_OSCNode->c_Properties.c_STWFlashloaderSettings.c_Data[
                                                               u32_ByteIndex], 16).toUpper());
                  }
                  else
                  {
                     c_Retval =
                        static_cast<sintn>(pc_OSCNode->c_Properties.c_STWFlashloaderSettings.c_Data[u32_ByteIndex]);
                  }
               }
               else
               {
                  if (osn_Role == static_cast<sintn>(Qt::DisplayRole))
                  {
                     c_Retval = "0x0";
                  }
                  else
                  {
                     c_Retval = 0;
                  }
               }
               break;
            case eBYTE4:
               u32_ByteIndex = 3;
               if (pc_OSCNode->c_Properties.c_STWFlashloaderSettings.c_Data.size() > u32_ByteIndex)
               {
                  if (osn_Role == static_cast<sintn>(Qt::DisplayRole))
                  {
                     c_Retval =
                        QString("0x%1").arg(QString::number(pc_OSCNode->c_Properties.c_STWFlashloaderSettings.c_Data[
                                                               u32_ByteIndex], 16).toUpper());
                  }
                  else
                  {
                     c_Retval =
                        static_cast<sintn>(pc_OSCNode->c_Properties.c_STWFlashloaderSettings.c_Data[u32_ByteIndex]);
                  }
               }
               else
               {
                  if (osn_Role == static_cast<sintn>(Qt::DisplayRole))
                  {
                     c_Retval = "0x0";
                  }
                  else
                  {
                     c_Retval = 0;
                  }
               }
               break;
            case eBYTE5:
               u32_ByteIndex = 4;
               if (pc_OSCNode->c_Properties.c_STWFlashloaderSettings.c_Data.size() > u32_ByteIndex)
               {
                  if (osn_Role == static_cast<sintn>(Qt::DisplayRole))
                  {
                     c_Retval =
                        QString("0x%1").arg(QString::number(pc_OSCNode->c_Properties.c_STWFlashloaderSettings.c_Data[
                                                               u32_ByteIndex], 16).toUpper());
                  }
                  else
                  {
                     c_Retval =
                        static_cast<sintn>(pc_OSCNode->c_Properties.c_STWFlashloaderSettings.c_Data[u32_ByteIndex]);
                  }
               }
               else
               {
                  if (osn_Role == static_cast<sintn>(Qt::DisplayRole))
                  {
                     c_Retval = "0x0";
                  }
                  else
                  {
                     c_Retval = 0;
                  }
               }
               break;
            case eBYTE6:
               u32_ByteIndex = 5;
               if (pc_OSCNode->c_Properties.c_STWFlashloaderSettings.c_Data.size() > u32_ByteIndex)
               {
                  if (osn_Role == static_cast<sintn>(Qt::DisplayRole))
                  {
                     c_Retval =
                        QString("0x%1").arg(QString::number(pc_OSCNode->c_Properties.c_STWFlashloaderSettings.c_Data[
                                                               u32_ByteIndex], 16).toUpper());
                  }
                  else
                  {
                     c_Retval =
                        static_cast<sintn>(pc_OSCNode->c_Properties.c_STWFlashloaderSettings.c_Data[u32_ByteIndex]);
                  }
               }
               else
               {
                  if (osn_Role == static_cast<sintn>(Qt::DisplayRole))
                  {
                     c_Retval = "0x0";
                  }
                  else
                  {
                     c_Retval = 0;
                  }
               }
               break;
            case eBYTE7:
               u32_ByteIndex = 6;
               if (pc_OSCNode->c_Properties.c_STWFlashloaderSettings.c_Data.size() > u32_ByteIndex)
               {
                  if (osn_Role == static_cast<sintn>(Qt::DisplayRole))
                  {
                     c_Retval =
                        QString("0x%1").arg(QString::number(pc_OSCNode->c_Properties.c_STWFlashloaderSettings.c_Data[
                                                               u32_ByteIndex], 16).toUpper());
                  }
                  else
                  {
                     c_Retval =
                        static_cast<sintn>(pc_OSCNode->c_Properties.c_STWFlashloaderSettings.c_Data[u32_ByteIndex]);
                  }
               }
               else
               {
                  if (osn_Role == static_cast<sintn>(Qt::DisplayRole))
                  {
                     c_Retval = "0x0";
                  }
                  else
                  {
                     c_Retval = 0;
                  }
               }
               break;
            case eBYTE8:
               u32_ByteIndex = 7;
               if (pc_OSCNode->c_Properties.c_STWFlashloaderSettings.c_Data.size() > u32_ByteIndex)
               {
                  if (osn_Role == static_cast<sintn>(Qt::DisplayRole))
                  {
                     c_Retval =
                        QString("0x%1").arg(QString::number(pc_OSCNode->c_Properties.c_STWFlashloaderSettings.c_Data[
                                                               u32_ByteIndex], 16).toUpper());
                  }
                  else
                  {
                     c_Retval =
                        static_cast<sintn>(pc_OSCNode->c_Properties.c_STWFlashloaderSettings.c_Data[u32_ByteIndex]);
                  }
               }
               else
               {
                  if (osn_Role == static_cast<sintn>(Qt::DisplayRole))
                  {
                     c_Retval = "0x0";
                  }
                  else
                  {
                     c_Retval = 0;
                  }
               }
               break;
            }
         }
      }
      else if (osn_Role == static_cast<sintn>(Qt::ForegroundRole))
      {
         const QColor c_Active = mc_STYLE_GUIDE_COLOR_6;
         const QColor c_Disabled = mc_STYLE_GUIDE_COLOR_10;
         const C_OSCNode * const pc_OSCNode = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
         if (pc_OSCNode != NULL)
         {
            if (pc_OSCNode->c_Properties.c_STWFlashloaderSettings.q_ResetMessageActive == false)
            {
               c_Retval = c_Disabled;
            }
            else
            {
               //Stylesheets do not allow access of specific columns so we need to set it manually
               switch (e_Col)
               {
               case eID:
               case eDLC:
               case eEXTENDED_FORMAT:
                  c_Retval = c_Active;
                  break;
               case eBYTE1:
                  if (pc_OSCNode->c_Properties.c_STWFlashloaderSettings.u8_ResetMessageDlc > 0)
                  {
                     c_Retval = c_Active;
                  }
                  else
                  {
                     c_Retval = c_Disabled;
                  }
                  break;
               case eBYTE2:
                  if (pc_OSCNode->c_Properties.c_STWFlashloaderSettings.u8_ResetMessageDlc > 1)
                  {
                     c_Retval = c_Active;
                  }
                  else
                  {
                     c_Retval = c_Disabled;
                  }
                  break;
               case eBYTE3:
                  if (pc_OSCNode->c_Properties.c_STWFlashloaderSettings.u8_ResetMessageDlc > 2)
                  {
                     c_Retval = c_Active;
                  }
                  else
                  {
                     c_Retval = c_Disabled;
                  }
                  break;
               case eBYTE4:
                  if (pc_OSCNode->c_Properties.c_STWFlashloaderSettings.u8_ResetMessageDlc > 3)
                  {
                     c_Retval = c_Active;
                  }
                  else
                  {
                     c_Retval = c_Disabled;
                  }
                  break;
               case eBYTE5:
                  if (pc_OSCNode->c_Properties.c_STWFlashloaderSettings.u8_ResetMessageDlc > 4)
                  {
                     c_Retval = c_Active;
                  }
                  else
                  {
                     c_Retval = c_Disabled;
                  }
                  break;
               case eBYTE6:
                  if (pc_OSCNode->c_Properties.c_STWFlashloaderSettings.u8_ResetMessageDlc > 5)
                  {
                     c_Retval = c_Active;
                  }
                  else
                  {
                     c_Retval = c_Disabled;
                  }
                  break;
               case eBYTE7:
                  if (pc_OSCNode->c_Properties.c_STWFlashloaderSettings.u8_ResetMessageDlc > 6)
                  {
                     c_Retval = c_Active;
                  }
                  else
                  {
                     c_Retval = c_Disabled;
                  }
                  break;
               case eBYTE8:
                  if (pc_OSCNode->c_Properties.c_STWFlashloaderSettings.u8_ResetMessageDlc > 7)
                  {
                     c_Retval = c_Active;
                  }
                  else
                  {
                     c_Retval = c_Disabled;
                  }
                  break;
               }
            }
         }
      }
      else if (osn_Role == static_cast<sintn>(Qt::TextAlignmentRole))
      {
         c_Retval = QVariant(static_cast<sintn>(Qt::AlignCenter));
      }
      else if (osn_Role == msn_USER_ROLE_CONDITIONAL_VALUE)
      {
         const C_OSCNode * pc_OSCNode;
         switch (e_Col)
         {
         case eID:
            pc_OSCNode = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
            if (pc_OSCNode != NULL)
            {
               if (pc_OSCNode->c_Properties.c_STWFlashloaderSettings.q_ResetMessageExtendedId == true)
               {
                  c_Retval = 1;
               }
               else
               {
                  c_Retval = 0;
               }
            }
            break;
         default:
            //not required
            break;
         }
      }
      else
      {
         //No special handling
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set data at index

   \param[in] orc_Index Index
   \param[in] orc_Value New data
   \param[in] osn_Role  Data role

   \return
   true  success
   false failure
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeSfoResetMessageTableModel::setData(const QModelIndex & orc_Index, const QVariant & orc_Value,
                                               const sintn osn_Role)
{
   bool q_Retval = false;

   if (data(orc_Index, osn_Role) != orc_Value)
   {
      if (osn_Role == static_cast<sintn>(Qt::EditRole))
      {
         const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
         if ((orc_Index.row() >= 0) && (pc_Node != NULL))
         {
            C_OSCNodeStwFlashloaderSettings c_Settings = pc_Node->c_Properties.c_STWFlashloaderSettings;
            const C_SdNdeSfoResetMessageTableModel::E_Columns e_Col = C_SdNdeSfoResetMessageTableModel::h_ColumnToEnum(
               orc_Index.column());
            switch (e_Col)
            {
            case eID:
               c_Settings.u32_ResetMessageId = orc_Value.toUInt();
               break;
            case eDLC:
               c_Settings.u8_ResetMessageDlc = static_cast<uint8>(orc_Value.toInt());
               break;
            case eEXTENDED_FORMAT:
               if (orc_Value.toInt() == C_SdNdeSfoResetMessageTableModel::mhsn_INDEX_STANDARD)
               {
                  c_Settings.q_ResetMessageExtendedId = false;
               }
               else
               {
                  c_Settings.q_ResetMessageExtendedId = true;
               }
               break;
            case eBYTE1:
               if (c_Settings.c_Data.size() == 0)
               {
                  c_Settings.c_Data.resize(1, 0);
               }
               c_Settings.c_Data[0] = static_cast<uint8>(orc_Value.toInt());
               break;
            case eBYTE2:
               if (c_Settings.c_Data.size() <= 1)
               {
                  c_Settings.c_Data.resize(2, 0);
               }
               c_Settings.c_Data[1] = static_cast<uint8>(orc_Value.toInt());
               break;
            case eBYTE3:
               if (c_Settings.c_Data.size() <= 2)
               {
                  c_Settings.c_Data.resize(3, 0);
               }
               c_Settings.c_Data[2] = static_cast<uint8>(orc_Value.toInt());
               break;
            case eBYTE4:
               if (c_Settings.c_Data.size() <= 3)
               {
                  c_Settings.c_Data.resize(4, 0);
               }
               c_Settings.c_Data[3] = static_cast<uint8>(orc_Value.toInt());
               break;
            case eBYTE5:
               if (c_Settings.c_Data.size() <= 4)
               {
                  c_Settings.c_Data.resize(5, 0);
               }
               c_Settings.c_Data[4] = static_cast<uint8>(orc_Value.toInt());
               break;
            case eBYTE6:
               if (c_Settings.c_Data.size() <= 5)
               {
                  c_Settings.c_Data.resize(6, 0);
               }
               c_Settings.c_Data[5] = static_cast<uint8>(orc_Value.toInt());
               break;
            case eBYTE7:
               if (c_Settings.c_Data.size() <= 6)
               {
                  c_Settings.c_Data.resize(7, 0);
               }
               c_Settings.c_Data[6] = static_cast<uint8>(orc_Value.toInt());
               break;
            case eBYTE8:
               if (c_Settings.c_Data.size() <= 7)
               {
                  c_Settings.c_Data.resize(8, 0);
               }
               c_Settings.c_Data[7] = static_cast<uint8>(orc_Value.toInt());
               break;
            default:
               break;
            }

            C_PuiSdHandler::h_GetInstance()->SetStwFlashloaderSettings(this->mu32_NodeIndex, c_Settings);

            //lint -e{1793} Qt example
            Q_EMIT this->dataChanged(orc_Index, orc_Index, QVector<stw_types::sintn>() << osn_Role);
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get flags for item

   \param[in] orc_Index Item

   \return
   Flags for item
*/
//----------------------------------------------------------------------------------------------------------------------
Qt::ItemFlags C_SdNdeSfoResetMessageTableModel::flags(const QModelIndex & orc_Index) const
{
   Qt::ItemFlags c_Retval;
   if (orc_Index.isValid() == true)
   {
      const C_OSCNode * pc_OSCNode;
      const C_SdNdeSfoResetMessageTableModel::E_Columns e_Col = C_SdNdeSfoResetMessageTableModel::h_ColumnToEnum(
         orc_Index.column());
      //Each item
      c_Retval = QAbstractTableModel::flags(orc_Index) | Qt::ItemIsEnabled;
      //Add edit
      switch (e_Col)
      {
      case eID:
      case eDLC:
      case eEXTENDED_FORMAT:
         c_Retval = c_Retval | Qt::ItemIsEditable;
         break;
      case eBYTE1:
         pc_OSCNode = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
         if ((pc_OSCNode != NULL) && (pc_OSCNode->c_Properties.c_STWFlashloaderSettings.u8_ResetMessageDlc > 0))
         {
            c_Retval = c_Retval | Qt::ItemIsEditable;
         }
         break;
      case eBYTE2:
         pc_OSCNode = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
         if ((pc_OSCNode != NULL) && (pc_OSCNode->c_Properties.c_STWFlashloaderSettings.u8_ResetMessageDlc > 1))
         {
            c_Retval = c_Retval | Qt::ItemIsEditable;
         }
         break;
      case eBYTE3:
         pc_OSCNode = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
         if ((pc_OSCNode != NULL) && (pc_OSCNode->c_Properties.c_STWFlashloaderSettings.u8_ResetMessageDlc > 2))
         {
            c_Retval = c_Retval | Qt::ItemIsEditable;
         }
         break;
      case eBYTE4:
         pc_OSCNode = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
         if ((pc_OSCNode != NULL) && (pc_OSCNode->c_Properties.c_STWFlashloaderSettings.u8_ResetMessageDlc > 3))
         {
            c_Retval = c_Retval | Qt::ItemIsEditable;
         }
         break;
      case eBYTE5:
         pc_OSCNode = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
         if ((pc_OSCNode != NULL) && (pc_OSCNode->c_Properties.c_STWFlashloaderSettings.u8_ResetMessageDlc > 4))
         {
            c_Retval = c_Retval | Qt::ItemIsEditable;
         }
         break;
      case eBYTE6:
         pc_OSCNode = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
         if ((pc_OSCNode != NULL) && (pc_OSCNode->c_Properties.c_STWFlashloaderSettings.u8_ResetMessageDlc > 5))
         {
            c_Retval = c_Retval | Qt::ItemIsEditable;
         }
         break;
      case eBYTE7:
         pc_OSCNode = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
         if ((pc_OSCNode != NULL) && (pc_OSCNode->c_Properties.c_STWFlashloaderSettings.u8_ResetMessageDlc > 6))
         {
            c_Retval = c_Retval | Qt::ItemIsEditable;
         }
         break;
      case eBYTE8:
         pc_OSCNode = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
         if ((pc_OSCNode != NULL) && (pc_OSCNode->c_Properties.c_STWFlashloaderSettings.u8_ResetMessageDlc > 7))
         {
            c_Retval = c_Retval | Qt::ItemIsEditable;
         }
         break;
      default:
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
C_SdNdeSfoResetMessageTableModel::E_Columns C_SdNdeSfoResetMessageTableModel::h_ColumnToEnum(const sint32 & ors32_Column)
{
   C_SdNdeSfoResetMessageTableModel::E_Columns e_Retval;
   switch (ors32_Column)
   {
   case 0:
      e_Retval = eID;
      break;
   case 1:
      e_Retval = eEXTENDED_FORMAT;
      break;
   case 2:
      e_Retval = eDLC;
      break;
   case 3:
      e_Retval = eBYTE1;
      break;
   case 4:
      e_Retval = eBYTE2;
      break;
   case 5:
      e_Retval = eBYTE3;
      break;
   case 6:
      e_Retval = eBYTE4;
      break;
   case 7:
      e_Retval = eBYTE5;
      break;
   case 8:
      e_Retval = eBYTE6;
      break;
   case 9:
      e_Retval = eBYTE7;
      break;
   case 10:
      e_Retval = eBYTE8;
      break;
   default:
      e_Retval = eBYTE1;
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
sint32 C_SdNdeSfoResetMessageTableModel::h_EnumToColumn(const C_SdNdeSfoResetMessageTableModel::E_Columns & ore_Value)
{
   sint32 s32_Retval;

   switch (ore_Value)
   {
   case eID:
      s32_Retval = 0;
      break;
   case eEXTENDED_FORMAT:
      s32_Retval = 1;
      break;
   case eDLC:
      s32_Retval = 2;
      break;
   case eBYTE1:
      s32_Retval = 3;
      break;
   case eBYTE2:
      s32_Retval = 4;
      break;
   case eBYTE3:
      s32_Retval = 5;
      break;
   case eBYTE4:
      s32_Retval = 6;
      break;
   case eBYTE5:
      s32_Retval = 7;
      break;
   case eBYTE6:
      s32_Retval = 8;
      break;
   case eBYTE7:
      s32_Retval = 9;
      break;
   case eBYTE8:
      s32_Retval = 10;
      break;
   default:
      s32_Retval = -1;
      break;
   }

   return s32_Retval;
}
