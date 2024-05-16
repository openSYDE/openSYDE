//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node data pool list data set model (implementation)

   Node data pool list data set model

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <iostream>
#include <QMimeData>
#include "C_SdNdeDpListDataSetModel.hpp"
#include "stwerrors.hpp"
#include "constants.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_GtGetText.hpp"
#include "C_SdClipBoardHelper.hpp"
#include "C_Uti.hpp"
#include "C_NagToolTip.hpp"
#include "C_SdNdeUnoUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui;
using namespace stw::errors;

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
C_SdNdeDpListDataSetModel::C_SdNdeDpListDataSetModel(QObject * const opc_Parent) :
   QAbstractTableModel(opc_Parent),
   mu32_NodeIndex(0),
   mu32_DataPoolIndex(0),
   mu32_ListIndex(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Specify associated list

   \param[in]  oru32_NodeIndex      Node index
   \param[in]  oru32_DataPoolIndex  Node data pool index
   \param[in]  oru32_ListIndex      Node data pool list index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListDataSetModel::SetList(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                        const uint32_t & oru32_ListIndex)
{
   beginResetModel();
   this->mu32_NodeIndex = oru32_NodeIndex;
   this->mu32_DataPoolIndex = oru32_DataPoolIndex;
   this->mu32_ListIndex = oru32_ListIndex;
   endResetModel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if equal

   \param[in]  oru32_NodeIndex      Node index
   \param[in]  oru32_DataPoolIndex  Node data pool index
   \param[in]  oru32_ListIndex      Node data pool list index

   \return
   True  Match
   False No match
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDpListDataSetModel::Equals(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                       const uint32_t & oru32_ListIndex) const
{
   bool q_Retval;

   if (((this->mu32_NodeIndex == oru32_NodeIndex) && (this->mu32_DataPoolIndex == oru32_DataPoolIndex)) &&
       (this->mu32_ListIndex == oru32_ListIndex))
   {
      q_Retval = true;
   }
   else
   {
      q_Retval = false;
   }
   return q_Retval;
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
QVariant C_SdNdeDpListDataSetModel::headerData(const int32_t os32_Section, const Qt::Orientation oe_Orientation,
                                               const int32_t os32_Role) const
{
   QVariant c_Retval = QAbstractTableModel::headerData(os32_Section, oe_Orientation, os32_Role);

   if (oe_Orientation == Qt::Orientation::Vertical)
   {
      const C_SdNdeDpListDataSetModel::E_Rows e_Row = h_RowToEnum(os32_Section);
      if (os32_Role == static_cast<int32_t>(Qt::DisplayRole))
      {
         switch (e_Row)
         {
         case eNAME:
            c_Retval = C_GtGetText::h_GetText("Name");
            break;
         case eCOMMENT:
            c_Retval = C_GtGetText::h_GetText("Comment");
            break;
         default:
            break;
         }
      }
      else if (os32_Role == static_cast<int32_t>(Qt::TextAlignmentRole))
      {
         switch (e_Row)
         {
         case eNAME:
            c_Retval = static_cast<QVariant>(Qt::AlignLeft | Qt::AlignVCenter);
            break;
         case eCOMMENT:
            c_Retval = static_cast<QVariant>(Qt::AlignLeft | Qt::AlignTop);
            break;
         default:
            break;
         }
      }
      else
      {
         //No special handling
      }
   }
   else
   {
      if (os32_Role == static_cast<int32_t>(Qt::DisplayRole))
      {
         c_Retval = QString::number(os32_Section + 1);
      }
      else if (os32_Role == static_cast<int32_t>(Qt::TextAlignmentRole))
      {
         c_Retval = static_cast<QVariant>(Qt::AlignHCenter | Qt::AlignVCenter);
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
int32_t C_SdNdeDpListDataSetModel::rowCount(const QModelIndex & orc_Parent) const
{
   int32_t s32_Retval = 0;

   if (!orc_Parent.isValid())
   {
      //For table parent should always be invalid
      s32_Retval = 2;
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
int32_t C_SdNdeDpListDataSetModel::columnCount(const QModelIndex & orc_Parent) const
{
   int32_t s32_Retval = 0;

   if (!orc_Parent.isValid())
   {
      const C_OscNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOscDataPoolList(
         this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex);
      if (pc_List != NULL)
      {
         //For table parent should always be invalid
         s32_Retval = pc_List->c_DataSets.size();
      }
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
QVariant C_SdNdeDpListDataSetModel::data(const QModelIndex & orc_Index, const int32_t os32_Role) const
{
   QVariant c_Retval;

   if (orc_Index.isValid() == true)
   {
      const C_SdNdeDpListDataSetModel::E_Rows e_Col = h_RowToEnum(orc_Index.row());
      if ((os32_Role == static_cast<int32_t>(Qt::DisplayRole)) || (os32_Role == static_cast<int32_t>(Qt::EditRole)))
      {
         const C_OscNodeDataPoolList * const pc_OscElement =
            C_PuiSdHandler::h_GetInstance()->GetOscDataPoolList(this->mu32_NodeIndex,
                                                                this->mu32_DataPoolIndex,
                                                                this->mu32_ListIndex);
         if (pc_OscElement != NULL)
         {
            const uint32_t u32_Index = static_cast<uint32_t>(orc_Index.column());
            if (u32_Index < pc_OscElement->c_DataSets.size())
            {
               const C_OscNodeDataPoolDataSet & rc_DataSet = pc_OscElement->c_DataSets[u32_Index];
               switch (e_Col)
               {
               case eNAME:
                  c_Retval = static_cast<QString>(rc_DataSet.c_Name.c_str());
                  break;
               case eCOMMENT:
                  c_Retval = static_cast<QString>(rc_DataSet.c_Comment.c_str());
                  break;
               default:
                  break;
               }
            }
         }
      }
      else if (os32_Role == static_cast<int32_t>(Qt::TextAlignmentRole))
      {
         switch (e_Col)
         {
         case eNAME:
            c_Retval = static_cast<QVariant>(Qt::AlignLeft | Qt::AlignVCenter);
            break;
         case eCOMMENT:
            c_Retval = static_cast<QVariant>(Qt::AlignLeft | Qt::AlignTop);
            break;
         default:
            break;
         }
      }
      else if (os32_Role == static_cast<int32_t>(Qt::ForegroundRole))
      {
         //Stylesheets do not allow access of specific columns so we need to set t manually
         switch (e_Col)
         {
         case eCOMMENT:
            c_Retval = mc_STYLE_GUIDE_COLOR_6;
            break;
         case eNAME:
            //Default
            c_Retval = mc_STYLE_GUIDE_COLOR_6;
            //Check error
            if (this->data(orc_Index, ms32_USER_ROLE_ERROR).toBool() == true)
            {
               c_Retval = mc_STYLE_GUIDE_COLOR_18;
            }
            break;
         default:
            c_Retval = mc_STYLE_GUIDE_COLOR_6;
            break;
         }
      }
      else if (os32_Role == ms32_USER_ROLE_TOOL_TIP_HEADING)
      {
         c_Retval = "";
         if (e_Col == eNAME)
         {
            if (this->data(orc_Index, ms32_USER_ROLE_ERROR).toBool() == true)
            {
               c_Retval = C_GtGetText::h_GetText("Dataset Name");
            }
         }
         else
         {
            // default already set to ""
         }
      }
      else if (os32_Role == ms32_USER_ROLE_TOOL_TIP_TYPE)
      {
         if (e_Col == eNAME)
         {
            if (this->data(orc_Index, ms32_USER_ROLE_ERROR).toBool() == true)
            {
               c_Retval = static_cast<int32_t>(C_NagToolTip::eERROR);
            }
            else
            {
               c_Retval = static_cast<int32_t>(C_NagToolTip::eDEFAULT);
            }
         }
         else
         {
            c_Retval = static_cast<int32_t>(C_NagToolTip::eDEFAULT);
         }
      }
      else if (os32_Role == ms32_USER_ROLE_TOOL_TIP_CONTENT)
      {
         c_Retval = "";
         if (e_Col == eNAME)
         {
            //Check error
            QString c_Output;
            const C_OscNodeDataPoolList * const pc_OscList = C_PuiSdHandler::h_GetInstance()->GetOscDataPoolList(
               this->mu32_NodeIndex,
               this->mu32_DataPoolIndex,
               this->mu32_ListIndex);
            if (pc_OscList != NULL)
            {
               bool q_NameInvalid = false;
               bool q_NameConflict = false;
               pc_OscList->CheckErrorDataSet(static_cast<uint32_t>(orc_Index.column()), &q_NameConflict,
                                             &q_NameInvalid);
               if (q_NameInvalid == true)
               {
                  c_Output += C_GtGetText::h_GetText("- is empty or contains invalid characters\n");
               }
               if (q_NameConflict == true)
               {
                  c_Output += C_GtGetText::h_GetText("- is already in use\n");
               }
            }
            c_Retval = c_Output;
         }
         else
         {
            // default already set to ""
         }
      }
      else if (os32_Role == ms32_USER_ROLE_ERROR)
      {
         if (e_Col == eNAME)
         {
            //Default
            c_Retval = false;
            bool q_NameInvalid = false;
            const C_OscNodeDataPoolList * const pc_OscList = C_PuiSdHandler::h_GetInstance()->GetOscDataPoolList(
               this->mu32_NodeIndex,
               this->mu32_DataPoolIndex,
               this->mu32_ListIndex);
            if (pc_OscList != NULL)
            {
               bool q_NameConflict = false;
               pc_OscList->CheckErrorDataSet(static_cast<uint32_t>(orc_Index.column()), &q_NameConflict,
                                             &q_NameInvalid);
               if (q_NameConflict == true)
               {
                  q_NameInvalid = true;
               }
            }
            if (q_NameInvalid == true)
            {
               c_Retval = true;
            }
         }
         else
         {
            c_Retval = false;
         }
      }
      else
      {
         //No handling necessary
      }
   }
   return c_Retval;
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
bool C_SdNdeDpListDataSetModel::setData(const QModelIndex & orc_Index, const QVariant & orc_Value,
                                        const int32_t os32_Role)
{
   bool q_Retval = false;

   if (data(orc_Index, os32_Role) != orc_Value)
   {
      if (os32_Role == static_cast<int32_t>(Qt::EditRole))
      {
         if (orc_Index.column() >= 0)
         {
            const uint32_t u32_Index = static_cast<uint32_t>(orc_Index.column());
            const C_SdNdeDpListDataSetModel::E_Rows e_Row =
               h_RowToEnum(orc_Index.row());
            switch (e_Row)
            {
            case eNAME:
               Q_EMIT this->SigDataChange(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                          this->mu32_ListIndex,
                                          u32_Index, orc_Value,
                                          C_SdNdeDpUtil::E_DataSetDataChangeType::eDATA_SET_NAME);
               q_Retval = true;
               break;
            case eCOMMENT:
               Q_EMIT this->SigDataChange(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                          this->mu32_ListIndex,
                                          u32_Index, orc_Value,
                                          C_SdNdeDpUtil::E_DataSetDataChangeType::eDATA_SET_COMMENT);
               q_Retval = true;
               break;
            default:
               break;
            }

            Q_EMIT this->dataChanged(orc_Index, orc_Index, QVector<int32_t>() << os32_Role);
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
Qt::ItemFlags C_SdNdeDpListDataSetModel::flags(const QModelIndex & orc_Index) const
{
   Qt::ItemFlags c_Retval;
   if (orc_Index.isValid() == true)
   {
      const C_SdNdeDpListDataSetModel::E_Rows e_Row = h_RowToEnum(orc_Index.row());
      //Each item
      c_Retval = QAbstractTableModel::flags(orc_Index) | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
      //Add edit
      switch (e_Row)
      {
      case eNAME:
      case eCOMMENT:
         c_Retval = c_Retval | Qt::ItemIsEditable;
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
/*! \brief   Get supported drop actions

   \return
   Supported drop actions
*/
//----------------------------------------------------------------------------------------------------------------------
Qt::DropActions C_SdNdeDpListDataSetModel::supportedDropActions(void) const
{
   return Qt::MoveAction;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Insert columns with content

   \param[in]  orc_OscNames            Inserted elements OSC names
   \param[in]  orc_OscDataSetValues    Inserted elements OSC data set values
   \param[in]  oru32_Row               Starting Row
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListDataSetModel::DoInsertColumns(const std::vector<C_OscNodeDataPoolDataSet> & orc_OscNames,
                                                const std::vector<std::vector<C_OscNodeDataPoolContent> > & orc_OscDataSetValues,
                                                const uint32_t & oru32_Row)
{
   if (orc_OscNames.size() == orc_OscDataSetValues.size())
   {
      this->mc_OscNames = orc_OscNames;
      this->mc_OscDataSetValues = orc_OscDataSetValues;
      this->insertColumns(oru32_Row, orc_OscNames.size());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Insert items into model

   \param[in]  os32_Column  Starting column
   \param[in]  os32_Count   Number of inserted items
   \param[in]  orc_Parent  Parent

   \return
   true  success
   false failure
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDpListDataSetModel::insertColumns(const int32_t os32_Column, const int32_t os32_Count,
                                              const QModelIndex & orc_Parent)
{
   bool q_Retval = false;

   if ((os32_Count > 0) && (os32_Column >= 0))
   {
      if ((this->mc_OscNames.size() == static_cast<uint32_t>(os32_Count)) &&
          (this->mc_OscDataSetValues.size() == static_cast<uint32_t>(os32_Count)))
      {
         beginInsertColumns(orc_Parent, os32_Column, os32_Column + (os32_Count - 1));
         for (uint32_t u32_Index = 0; u32_Index < static_cast<uint32_t>(os32_Count); ++u32_Index)
         {
            C_PuiSdHandler::h_GetInstance()->InsertDataPoolListDataSet(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                                       this->mu32_ListIndex,
                                                                       static_cast<uint32_t>(os32_Column) + u32_Index,
                                                                       this->mc_OscNames[u32_Index],
                                                                       this->mc_OscDataSetValues[u32_Index]);
         }
         endInsertColumns();
         Q_EMIT this->SigColumnCountChange(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                           this->mu32_ListIndex, this->columnCount());
         q_Retval =  true;
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove items from model

   \param[in]  os32_Column  Starting column
   \param[in]  os32_Count   Number of removed items
   \param[in]  orc_Parent  Parent

   \return
   true  success
   false failure
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDpListDataSetModel::removeColumns(const int32_t os32_Column, const int32_t os32_Count,
                                              const QModelIndex & orc_Parent)
{
   bool q_Retval = false;

   if ((os32_Count > 0) && (os32_Column >= 0))
   {
      beginRemoveColumns(orc_Parent, os32_Column, os32_Column + (os32_Count - 1));
      for (uint32_t u32_Index = static_cast<uint32_t>(os32_Column + os32_Count);
           u32_Index > static_cast<uint32_t>(os32_Column);
           --u32_Index)
      {
         C_PuiSdHandler::h_GetInstance()->RemoveDataPoolListDataSet(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                                    this->mu32_ListIndex,
                                                                    u32_Index - 1);
      }
      endRemoveColumns();
      Q_EMIT this->SigColumnCountChange(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                        this->mu32_ListIndex, this->columnCount());
      q_Retval = true;
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Move rows

   \param[in]  orc_SourceParent        Source parent
   \param[in]  os32_SourceColumn       Source column
   \param[in]  os32_Count              Count
   \param[in]  orc_DestinationParent   Destination parent
   \param[in]  os32_DestinationChild   Destination child

   \return
   true  success
   false failure
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDpListDataSetModel::moveColumns(const QModelIndex & orc_SourceParent, const int32_t os32_SourceColumn,
                                            const int32_t os32_Count, const QModelIndex & orc_DestinationParent,
                                            const int32_t os32_DestinationChild)
{
   bool q_Retval = false;

   if (os32_Count > 0)
   {
      beginMoveColumns(orc_SourceParent, os32_SourceColumn, os32_Count, orc_DestinationParent, os32_DestinationChild);
      for (uint32_t u32_It = 0; u32_It < static_cast<uint32_t>(os32_Count); ++u32_It)
      {
         C_PuiSdHandler::h_GetInstance()->MoveDataPoolListDataSet(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                                  this->mu32_ListIndex,
                                                                  static_cast<uint32_t>(os32_SourceColumn) + u32_It,
                                                                  static_cast<uint32_t>(os32_DestinationChild) +
                                                                  u32_It);
      }
      endMoveColumns();
      q_Retval = true;
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Row to enum conversion

   \param[in]  ors32_Row   Column

   \return
   Enum value
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpListDataSetModel::E_Rows C_SdNdeDpListDataSetModel::h_RowToEnum(const int32_t & ors32_Row)
{
   C_SdNdeDpListDataSetModel::E_Rows e_Retval = eNAME;
   switch (ors32_Row)
   {
   case 0:
      e_Retval = eNAME;
      break;
   case 1:
      e_Retval = eCOMMENT;
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
int32_t C_SdNdeDpListDataSetModel::h_EnumToRow(const C_SdNdeDpListDataSetModel::E_Rows & ore_Value)
{
   int32_t s32_Retval;

   switch (ore_Value)
   {
   case eNAME:
      s32_Retval = 0;
      break;
   case eCOMMENT:
      s32_Retval = 1;
      break;
   default:
      s32_Retval = -1;
      break;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Mime types

   \return
   Table mime types
*/
//----------------------------------------------------------------------------------------------------------------------
QStringList C_SdNdeDpListDataSetModel::mimeTypes(void) const
{
   //Append original types (Default events only accept this type)
   QStringList c_Retval = QAbstractTableModel::mimeTypes();

   //Append own types last (Default events only accept Qt types at start)
   c_Retval.append("application/stw/opensyde/system/node/data-pool/list/data-set/elements");
   c_Retval.append("application/stw/opensyde/system/node/data-pool/list/data-set/indices");

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get mime data of specified indices

   \param[in]  orc_Indices    Indices

   \return
   Mime data of specified indices
*/
//----------------------------------------------------------------------------------------------------------------------
QMimeData * C_SdNdeDpListDataSetModel::mimeData(const QModelIndexList & orc_Indices) const
{
   QMimeData * pc_Retval = NULL;

   if (orc_Indices.size() > 0)
   {
      if (this->mimeTypes().size() > 2)
      {
         const C_OscNodeDataPoolList * const pc_OscList = C_PuiSdHandler::h_GetInstance()->GetOscDataPoolList(
            this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex);
         if (pc_OscList != NULL)
         {
            std::vector<uint32_t> c_Cols = C_SdNdeDpUtil::h_ConvertVector(orc_Indices, false);
            QString c_String;
            std::vector<C_OscNodeDataPoolDataSet> c_OscNames;
            std::vector<std::vector<C_OscNodeDataPoolContent> > c_OscDataSetValues;
            //Convert indices to unique rows
            C_Uti::h_Uniqueify(c_Cols);
            //Reserve
            c_OscNames.reserve(c_Cols.size());
            c_OscDataSetValues.reserve(c_Cols.size());
            //Copy relevant data
            for (uint32_t u32_ItDataSetIndex = 0; u32_ItDataSetIndex < c_Cols.size(); ++u32_ItDataSetIndex)
            {
               std::vector<C_OscNodeDataPoolContent> c_CurrentDataSetValues;
               c_OscNames.push_back(pc_OscList->c_DataSets.at(c_Cols[u32_ItDataSetIndex]));
               c_CurrentDataSetValues.reserve(pc_OscList->c_Elements.size());
               for (uint32_t u32_ItElementIndex = 0; u32_ItElementIndex < pc_OscList->c_Elements.size();
                    ++u32_ItElementIndex)
               {
                  const C_OscNodeDataPoolListElement & rc_Element = pc_OscList->c_Elements[u32_ItElementIndex];
                  if (u32_ItDataSetIndex < rc_Element.c_DataSetValues.size())
                  {
                     c_CurrentDataSetValues.push_back(rc_Element.c_DataSetValues.at(c_Cols[u32_ItDataSetIndex]));
                  }
               }
               c_OscDataSetValues.push_back(c_CurrentDataSetValues);
            }
            //Store in mime
            pc_Retval = new QMimeData();
            //Use default mime type so qt accepts these
            pc_Retval->setData(this->mimeTypes().at(0), "");
            C_SdClipBoardHelper::h_StoreDataPoolListDataSetsToString(c_OscNames, c_OscDataSetValues, c_String);
            pc_Retval->setData(this->mimeTypes().at(1), c_String.toStdString().c_str());
            C_SdClipBoardHelper::h_StoreIndicesToString(c_Cols, c_String);
            pc_Retval->setData(this->mimeTypes().at(2), c_String.toStdString().c_str());
         }
      }
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Signal model update

   \param[in]  oru32_Column      Changed column
   \param[in]  ore_ChangeType    Change type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListDataSetModel::HandleDataChange(const uint32_t & oru32_Column,
                                                 const C_SdNdeDpUtil::E_DataSetDataChangeType & ore_ChangeType)
{
   int32_t s32_Row = -1;

   switch (ore_ChangeType)
   {
   case C_SdNdeDpUtil::eDATA_SET_NAME:
      s32_Row = C_SdNdeDpListDataSetModel::h_EnumToRow(C_SdNdeDpListDataSetModel::E_Rows::eNAME);
      Q_EMIT this->SigErrorChangePossible(this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex);
      break;
   case C_SdNdeDpUtil::eDATA_SET_COMMENT:
      s32_Row = C_SdNdeDpListDataSetModel::h_EnumToRow(C_SdNdeDpListDataSetModel::E_Rows::eCOMMENT);
      break;
   default:
      //No handling
      break;
   }
   if (s32_Row >= 0)
   {
      Q_EMIT this->dataChanged(this->index(s32_Row, oru32_Column), this->index(s32_Row, oru32_Column));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Register model data reset
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListDataSetModel::Reset(void)
{
   this->beginResetModel();
   this->endResetModel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Register error change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListDataSetModel::HandleErrorChange(void)
{
   Q_EMIT this->SigErrorChangePossible(this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check name

   \param[in]  oru32_Index    Data set index
   \param[in]  orc_Name       Name

   \return
   true  Available
   false Already in use
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDpListDataSetModel::CheckName(const uint32_t & oru32_Index, const QString & orc_Name) const
{
   return C_PuiSdHandler::h_GetInstance()->CheckNodeDataPoolListDataSetNameAvailable(this->mu32_NodeIndex,
                                                                                     this->mu32_DataPoolIndex,
                                                                                     this->mu32_ListIndex,
                                                                                     orc_Name.toStdString().c_str(),
                                                                                     &oru32_Index);
}
