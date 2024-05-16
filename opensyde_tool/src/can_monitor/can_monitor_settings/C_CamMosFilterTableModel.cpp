//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Model component for filter item table (implementation)

   Model component for filter item table.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QIcon>
#include <QFileInfo>

#include "stwtypes.hpp"
#include "constants.hpp"
#include "C_CamMosFilterTableModel.hpp"
#include "C_GtGetText.hpp"
#include "C_CamProHandler.hpp"
#include "C_CamDbHandler.hpp"
#include "C_Uti.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMosFilterTableModel::C_CamMosFilterTableModel(QObject * const opc_Parent) :
   C_TblModelAction(opc_Parent)
{
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
QVariant C_CamMosFilterTableModel::headerData(const int32_t os32_Section, const Qt::Orientation oe_Orientation,
                                              const int32_t os32_Role) const
{
   QVariant c_Retval = QAbstractTableModel::headerData(os32_Section, oe_Orientation, os32_Role);

   if (oe_Orientation == Qt::Orientation::Horizontal)
   {
      const C_CamMosFilterTableModel::E_Columns e_Col = h_ColumnToEnum(os32_Section);
      if ((os32_Role == static_cast<int32_t>(Qt::DisplayRole)) || (os32_Role == ms32_USER_ROLE_TOOL_TIP_HEADING))
      {
         switch (e_Col)
         {
         case eENABLED:
            c_Retval = C_GtGetText::h_GetText("Enabled");
            break;
         case eTYPE:
            c_Retval = C_GtGetText::h_GetText("Type");
            break;
         case eMODE:
            c_Retval = C_GtGetText::h_GetText("Mode");
            break;
         case eDETAILS:
            c_Retval = C_GtGetText::h_GetText("Details");
            break;
         case eREMOVE:
            c_Retval = C_GtGetText::h_GetText("Remove");
            break;
         default:
            break;
         }
      }
      else if (os32_Role == static_cast<int32_t>(Qt::TextAlignmentRole))
      {
         c_Retval = static_cast<QVariant>(Qt::AlignLeft | Qt::AlignVCenter);
      }
      else if (os32_Role == ms32_USER_ROLE_TOOL_TIP_CONTENT)
      {
         switch (e_Col)
         {
         case eENABLED:
            c_Retval = C_GtGetText::h_GetText("Activate or deactivate filter item.");
            break;
         case eTYPE:
            c_Retval = C_GtGetText::h_GetText("Available types: CAN ID and CAN ID Range.");
            break;
         case eMODE:
            c_Retval = C_GtGetText::h_GetText("Pass or stop given CAN ID(s).");
            break;
         case eDETAILS:
            c_Retval = C_GtGetText::h_GetText("CAN ID(s) to filter.");
            break;
         case eREMOVE:
            c_Retval = C_GtGetText::h_GetText("Remove selected filter item.");
            break;
         default:
            break;
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
/*! \brief   Get table column count

   \param[in]  orc_Parent  Parent

   \return
   Column count
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CamMosFilterTableModel::columnCount(const QModelIndex & orc_Parent) const
{
   int32_t s32_Retval = 0;

   if (!orc_Parent.isValid())
   {
      //For table parent should always be invalid
      s32_Retval = 5;
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
QVariant C_CamMosFilterTableModel::data(const QModelIndex & orc_Index, const int32_t os32_Role) const
{
   QVariant c_Retval;

   if (orc_Index.isValid() == true)
   {
      const uint32_t u32_Index = static_cast<uint32_t>(orc_Index.row());
      const C_CamProFilterItemData c_FilterItem = mc_FilterItemsData[u32_Index];
      const C_CamMosFilterTableModel::E_Columns e_Col = h_ColumnToEnum(orc_Index.column());
      if ((os32_Role == static_cast<int32_t>(Qt::DisplayRole)) || (os32_Role == static_cast<int32_t>(Qt::EditRole)))
      {
         QString c_TempExtended = "";
         QString c_TempDatabase = "";

         switch (e_Col)
         {
         case eENABLED:
            //Different handling for Boolean
            break;
         case eTYPE:
            switch (c_FilterItem.e_Type)
            {
            case C_CamProFilterItemData::eID_SINGLE:
               c_Retval = C_GtGetText::h_GetText("CAN ID");
               break;
            case C_CamProFilterItemData::eID_RANGE:
               c_Retval = C_GtGetText::h_GetText("CAN ID Range");
               break;
            default:
               break;
            }
            break;
         case eMODE:
            if (c_FilterItem.q_PassFilter == true)
            {
               c_Retval = C_GtGetText::h_GetText("Pass");
            }
            else
            {
               c_Retval = C_GtGetText::h_GetText("Stop");
            }
            break;
         case eDETAILS:
            // add "x" to ID if extended
            if (static_cast<bool>(c_FilterItem.u8_ExtendedId) ==  true)
            {
               c_TempExtended = "x";
            }
            if (c_FilterItem.e_Type == C_CamProFilterItemData::eID_SINGLE)
            {
               if (c_FilterItem.c_DatabaseMatch != "")
               {
                  c_TempDatabase = " (" + c_FilterItem.c_DatabaseMatch + ")";
               }

               c_Retval = C_Uti::h_GetValueAsHex(c_FilterItem.u32_StartId) + c_TempExtended + c_TempDatabase;
            }
            else
            {
               c_Retval = static_cast<QString>(C_GtGetText::h_GetText("From %1 to %2")).
                          arg(C_Uti::h_GetValueAsHex(c_FilterItem.u32_StartId) + c_TempExtended).
                          arg(C_Uti::h_GetValueAsHex(c_FilterItem.u32_EndId) + c_TempExtended);
            }
            break;
         case eREMOVE:
            // no text
            break;
         default:
            break;
         }
      }
      else if (os32_Role == ms32_USER_ROLE_INTERACTION_IS_LINK)
      {
         switch (e_Col)
         {
         case eREMOVE:
            c_Retval = true;
            break;
         case eENABLED:
         case eTYPE:
         case eMODE:
         case eDETAILS:
         default:
            c_Retval = false;
            break;
         }
      }
      else if (os32_Role == static_cast<int32_t>(Qt::CheckStateRole))
      {
         switch (e_Col)
         {
         case eTYPE:
         case eMODE:
         case eDETAILS:
         case eREMOVE:
            //Boolean only
            break;
         case eENABLED:
            c_Retval = mh_GetBoolAsCheckStateVariant(c_FilterItem.q_Enabled);
            break;
         default:
            break;
         }
      }
      else if (os32_Role == ms32_USER_ROLE_ICON)
      {
         QStringList c_Icons;
         switch (e_Col)
         {
         case eTYPE:
         case eMODE:
         case eDETAILS:
         case eENABLED:
            // no icon
            break;
         case eREMOVE:
            // first string is normal icon, second string selected icon
            c_Icons.append("://images/IconClearNoPadding.svg");
            c_Icons.append("://images/IconClearGrayHovered.svg");
            c_Retval = c_Icons;
            break;
         default:
            break;
         }
      }
      else if (os32_Role == ms32_USER_ROLE_TOOL_TIP_CONTENT)
      {
         switch (e_Col)
         {
         case eENABLED:
         case eTYPE:
         case eMODE:
         case eREMOVE:
            // no tooltip
            break;
         case eDETAILS:
            c_Retval = data(orc_Index, static_cast<int32_t>(Qt::DisplayRole));
            break;
         default:
            break;
         }
      }
      else
      {
         //Not necessary yet
      }
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle copy items action

   \param[in]  orc_SelectedIndices  Selected row indices (Expected: unique)
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{9175}  nothing needs to be done here
void C_CamMosFilterTableModel::CopySelectedItems(const std::vector<uint32_t> & orc_SelectedIndices) const
{
   // nothing needs to be done here
   Q_UNUSED(orc_SelectedIndices)
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set data at index

   \param[in]  orc_Index   Index
   \param[in]  orc_Value   New data
   \param[in]  os32_Role    Data role

   \return
   true  success
   false failure
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_CamMosFilterTableModel::setData(const QModelIndex & orc_Index, const QVariant & orc_Value,
                                       const int32_t os32_Role)
{
   bool q_Retval = false;

   if (data(orc_Index, os32_Role) != orc_Value)
   {
      if (orc_Index.isValid() == true)
      {
         const int32_t s32_ColumnDetails = h_EnumToColumn(eDETAILS);
         const uint32_t u32_RowIndex = static_cast<uint32_t>(orc_Index.row());
         const C_CamMosFilterTableModel::E_Columns e_Col = h_ColumnToEnum(orc_Index.column());
         if (u32_RowIndex < static_cast<uint32_t>(this->mc_FilterItemsData.size()))
         {
            C_CamProFilterItemData & rc_FilterItemData = mc_FilterItemsData[u32_RowIndex];

            if (os32_Role == static_cast<int32_t>(Qt::CheckStateRole))
            {
               if (orc_Value.type() == QVariant::Int)
               {
                  switch (e_Col)
                  {
                  case eTYPE:
                  case eMODE:
                  case eDETAILS:
                  case eREMOVE:
                     //Boolean only
                     break;
                  case eENABLED:
                     rc_FilterItemData.q_Enabled = mh_GetCheckStateVariantAsBool(orc_Value);
                     q_Retval = true;
                     break;
                  default:
                     break;
                  }
               }
            }
            else if (os32_Role ==  static_cast<int32_t>(Qt::DisplayRole))
            {
               switch (e_Col)
               {
               case eTYPE:
                  rc_FilterItemData.e_Type = static_cast<C_CamProFilterItemData::E_Type>(orc_Value.toInt());
                  // force equality of start and end ID if type is "ID"
                  if (rc_FilterItemData.e_Type == C_CamProFilterItemData::E_Type::eID_SINGLE)
                  {
                     rc_FilterItemData.u32_EndId = rc_FilterItemData.u32_StartId;
                  }
                  // update column "Details"
                  //lint -e{1793} Qt example
                  Q_EMIT (this->dataChanged(this->index(u32_RowIndex, s32_ColumnDetails),
                                            this->index(u32_RowIndex, s32_ColumnDetails),
                                            QVector<int32_t>() << os32_Role));
                  q_Retval = true; // for updating data in column "Type"
                  break;
               case eMODE:
                  rc_FilterItemData.q_PassFilter = orc_Value.toBool();
                  q_Retval = true;
                  break;
               case eDETAILS:
               // Different handling for details because here is more information than a single value contained
               case eREMOVE:
               // Different handling for button
               case eENABLED:
                  // Different handling for Boolean
                  break;
               default:
                  break;
               }
            }
            else
            {
               // nothing to do
            }

            if (q_Retval == true)
            {
               //lint -e{1793} Qt example
               Q_EMIT (this->dataChanged(orc_Index, orc_Index, QVector<int32_t>() << os32_Role));
            }
         }
      }
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get flags for item

   \param[in]  orc_Index   Item

   \return
   Flags for item
*/
//----------------------------------------------------------------------------------------------------------------------
Qt::ItemFlags C_CamMosFilterTableModel::flags(const QModelIndex & orc_Index) const
{
   Qt::ItemFlags c_Retval = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
   if (orc_Index.isValid() == true)
   {
      const C_CamMosFilterTableModel::E_Columns e_Col = h_ColumnToEnum(orc_Index.column());
      switch (e_Col)
      {
      case eTYPE:
      case eMODE:
      case eDETAILS:
         //No flag to add
         break;
      case eREMOVE:
         c_Retval = c_Retval | Qt::ItemIsEditable;
         break;
      case eENABLED:
         c_Retval = c_Retval | Qt::ItemIsUserCheckable;
         break;
      default:
         break;
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Column to enum conversion

   \param[in]  os32_Column    Column

   \return
   Enum value
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMosFilterTableModel::E_Columns C_CamMosFilterTableModel::h_ColumnToEnum(const int32_t os32_Column)
{
   C_CamMosFilterTableModel::E_Columns e_Retval;
   switch (os32_Column)
   {
   case 0:
      e_Retval = eENABLED;
      break;
   case 1:
      e_Retval = eTYPE;
      break;
   case 2:
      e_Retval = eMODE;
      break;
   case 3:
      e_Retval = eDETAILS;
      break;
   case 4:
      e_Retval = eREMOVE;
      break;
   default:
      e_Retval = eENABLED;
      break;
   }
   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Enum to column conversion

   \param[in]  oe_Value    Enum value

   \return
   Column
   -1 Error
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CamMosFilterTableModel::h_EnumToColumn(const C_CamMosFilterTableModel::E_Columns oe_Value)
{
   int32_t s32_Retval = -1;

   switch (oe_Value)
   {
   case eENABLED:
      s32_Retval = 0;
      break;
   case eTYPE:
      s32_Retval = 1;
      break;
   case eMODE:
      s32_Retval = 2;
      break;
   case eDETAILS:
      s32_Retval = 3;
      break;
   case eREMOVE:
      s32_Retval = 4;
      break;
   default:
      break;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Search in all loaded databases for messages with given CAN ID.

   \param[in]  ou32_CanId     message CAN ID
   \param[in]  oq_IsExtended  Is extended

   \return
   string <Database>::<MessageName> (empty string if not found)
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_CamMosFilterTableModel::h_SearchMessageInDatabases(const uint32_t ou32_CanId, const bool oq_IsExtended)
{
   QString c_Return = "";
   QString c_Database = "";
   QString c_Message = "";

   C_CamDbHandler::h_GetInstance()->FindMessageById(ou32_CanId, oq_IsExtended, c_Database, c_Message);

   if ((c_Database != "") && (c_Message != ""))
   {
      const QFileInfo c_FileInfo(c_Database);

      c_Return = c_FileInfo.completeBaseName() + "::" + c_Message;
   }

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get data of filter items

   \return
   list of filter items
*/
//----------------------------------------------------------------------------------------------------------------------
QList<C_CamProFilterItemData> C_CamMosFilterTableModel::GetFilterItemsData() const
{
   return this->mc_FilterItemsData;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set data of filter items

   \param[in]  orc_FilterItemsData  list of filter items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterTableModel::SetFilterItemsData(const QList<C_CamProFilterItemData> & orc_FilterItemsData)
{
   this->mc_FilterItemsData = orc_FilterItemsData;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief Set IDs of specific filter item.

   \param[in]  ou32_Index        index of selected filter item
   \param[in]  ou32_NewStartId   new start ID
   \param[in]  ou32_NewEndId     new end ID
   \param[in]  oq_IsExtended     Is extended
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterTableModel::SetFilterItemIds(const uint32_t ou32_Index, const uint32_t ou32_NewStartId,
                                                const uint32_t ou32_NewEndId, const bool oq_IsExtended)
{
   QVector<int32_t> c_Roles;
   const int32_t s32_Column = C_CamMosFilterTableModel::h_EnumToColumn(C_CamMosFilterTableModel::eDETAILS);

   if (ou32_Index < static_cast<uint32_t>(this->mc_FilterItemsData.size()))
   {
      C_CamProFilterItemData & rc_FilterItemData = this->mc_FilterItemsData[ou32_Index];

      rc_FilterItemData.u32_StartId = ou32_NewStartId;
      rc_FilterItemData.u8_ExtendedId = static_cast<uint8_t>(oq_IsExtended);

      // force equality of start and end ID if type is "ID"
      if (rc_FilterItemData.e_Type == C_CamProFilterItemData::E_Type::eID_SINGLE)
      {
         rc_FilterItemData.u32_EndId = ou32_NewStartId;
         rc_FilterItemData.c_DatabaseMatch = C_CamMosFilterTableModel::h_SearchMessageInDatabases(ou32_NewStartId,
                                                                                                  oq_IsExtended);
      }
      else
      {
         rc_FilterItemData.u32_EndId = ou32_NewEndId;
      }
   }

   // update column "Details" of selected role (only display role necessary)
   c_Roles.push_back(static_cast<int32_t>(Qt::DisplayRole));
   Q_EMIT (this->dataChanged(this->index(ou32_Index, s32_Column), this->index(ou32_Index, s32_Column), c_Roles));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set extended ID flag of specific filter item.

   \param[in]  ou32_Index     index of selected filter item
   \param[in]  oq_Extended    extended ID flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterTableModel::SetFilterItemExtended(const uint32_t ou32_Index, const bool oq_Extended)
{
   if (ou32_Index < static_cast<uint32_t>(this->mc_FilterItemsData.size()))
   {
      const C_CamProFilterItemData & rc_FilterItemData = this->mc_FilterItemsData[ou32_Index];
      this->SetFilterItemIds(ou32_Index, rc_FilterItemData.u32_StartId, rc_FilterItemData.u32_EndId, oq_Extended);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle add new item action

   \param[in]  ou32_SelectedIndex   Index to insert item at

   \return
   Index of new item
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_CamMosFilterTableModel::m_AddNewItem(const uint32_t ou32_SelectedIndex)
{
   // add new filter item
   C_CamProFilterItemData c_NewFilterItem;
   uint32_t u32_Index;

   // set default type to ID range (even if default is in fact of type single ID "from 0 to 0")
   c_NewFilterItem.e_Type = C_CamProFilterItemData::eID_RANGE;

   // use valid index
   if (ou32_SelectedIndex < static_cast<uint32_t>(mc_FilterItemsData.size()))
   {
      u32_Index = ou32_SelectedIndex + 1UL;
   }
   else
   {
      u32_Index = mc_FilterItemsData.size();
   }

   // add in table and data handling
   this->beginInsertRows(QModelIndex(), u32_Index, u32_Index);
   this->mc_FilterItemsData.push_back(c_NewFilterItem);
   this->endInsertRows();

   return u32_Index;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle paste items action

   \param[in]  ou32_SelectedIndex   Index to insert item at

   \return
   Indices of new items
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<uint32_t> C_CamMosFilterTableModel::m_PasteItems(const uint32_t ou32_SelectedIndex)
{
   const std::vector<uint32_t> c_Retval;

   // nothing needs to be done here
   Q_UNUSED(ou32_SelectedIndex)
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get size of item container

   \return
   Size of item container
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_CamMosFilterTableModel::m_GetSizeItems() const
{
   return this->mc_FilterItemsData.size();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete specific item

   Warning: not expected to fail

   \param[in]  ou32_Index  Index to delete
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterTableModel::m_DeleteItem(const uint32_t ou32_Index)
{
   this->mc_FilterItemsData.removeAt(ou32_Index);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Move specific item

   For implementation: First delete ou32_SourceIndex then insert ou32_TargetIndex
   Warning: not expected to fail

   \param[in]  ou32_SourceIndex  Source index
   \param[in]  ou32_TargetIndex  Target index
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{9175}  nothing needs to be done here
void C_CamMosFilterTableModel::m_MoveItem(const uint32_t ou32_SourceIndex, const uint32_t ou32_TargetIndex)
{
   // nothing needs to be done here
   Q_UNUSED(ou32_SourceIndex)
   Q_UNUSED(ou32_TargetIndex)
}
