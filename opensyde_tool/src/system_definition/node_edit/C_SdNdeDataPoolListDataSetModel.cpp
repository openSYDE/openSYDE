//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Node data pool list data set model (implementation)

   Node data pool list data set model

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     08.02.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <iostream>
#include <QMimeData>
#include "C_SdNdeDataPoolListDataSetModel.h"
#include "stwerrors.h"
#include "constants.h"
#include "C_PuiSdHandler.h"
#include "C_GtGetText.h"
#include "C_SdClipBoardHelper.h"
#include "C_Uti.h"
#include "C_NagToolTip.h"
#include "C_SdNdeUnoUtil.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui;
using namespace stw_errors;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent

   \created     08.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdNdeDataPoolListDataSetModel::C_SdNdeDataPoolListDataSetModel(QObject * const opc_Parent) :
   QAbstractTableModel(opc_Parent),
   mu32_NodeIndex(0),
   mu32_DataPoolIndex(0),
   mu32_ListIndex(0)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Specify associated list

   \param[in] oru32_NodeIndex     Node index
   \param[in] oru32_DataPoolIndex Node data pool index
   \param[in] oru32_ListIndex     Node data pool list index

   \created     07.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListDataSetModel::SetList(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex,
                                              const uint32 & oru32_ListIndex)
{
   beginResetModel();
   this->mu32_NodeIndex = oru32_NodeIndex;
   this->mu32_DataPoolIndex = oru32_DataPoolIndex;
   this->mu32_ListIndex = oru32_ListIndex;
   endResetModel();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check if equal

   \param[in] oru32_NodeIndex     Node index
   \param[in] oru32_DataPoolIndex Node data pool index
   \param[in] oru32_ListIndex     Node data pool list index

   \return
   True  Match
   False No match

   \created     17.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_SdNdeDataPoolListDataSetModel::Equals(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex,
                                             const uint32 & oru32_ListIndex) const
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

//-----------------------------------------------------------------------------
/*!
   \brief   Get header data

   \param[in] osn_Section    Section
   \param[in] oe_Orientation Orientation
   \param[in] osn_Role       Role

   \return
   Header string

   \created     09.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QVariant C_SdNdeDataPoolListDataSetModel::headerData(const sintn osn_Section, const Qt::Orientation oe_Orientation,
                                                     const sintn osn_Role) const
{
   QVariant c_Retval = QAbstractTableModel::headerData(osn_Section, oe_Orientation, osn_Role);

   if (oe_Orientation == Qt::Orientation::Vertical)
   {
      const C_SdNdeDataPoolListDataSetModel::E_Rows e_Row = h_RowToEnum(osn_Section);
      if (osn_Role == static_cast<sintn>(Qt::DisplayRole))
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
      else if (osn_Role == static_cast<sintn>(Qt::TextAlignmentRole))
      {
         switch (e_Row)
         {
         case eNAME:
            c_Retval = QVariant(Qt::AlignLeft | Qt::AlignVCenter);
            break;
         case eCOMMENT:
            c_Retval = QVariant(Qt::AlignLeft | Qt::AlignTop);
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
      if (osn_Role == static_cast<sintn>(Qt::DisplayRole))
      {
         c_Retval = QString::number(osn_Section + 1);
      }
      else if (osn_Role == static_cast<sintn>(Qt::TextAlignmentRole))
      {
         c_Retval = QVariant(Qt::AlignHCenter | Qt::AlignVCenter);
      }
      else
      {
         //No special handling
      }
   }
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get table row count

   \param[in] orc_Parent Parent

   \return
   Row count

   \created     09.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sintn C_SdNdeDataPoolListDataSetModel::rowCount(const QModelIndex & orc_Parent) const
{
   sintn sn_Retval = 0;

   if (!orc_Parent.isValid())
   {
      //For table parent should always be invalid
      sn_Retval = 2;
   }
   return sn_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get table column count

   \param[in] orc_Parent Parent

   \return
   Column count

   \created     09.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sintn C_SdNdeDataPoolListDataSetModel::columnCount(const QModelIndex & orc_Parent) const
{
   stw_types::sintn sn_Retval = 0;
   if (!orc_Parent.isValid())
   {
      const C_OSCNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(
         this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex);
      if (pc_List != NULL)
      {
         //For table parent should always be invalid
         sn_Retval = pc_List->c_DataSets.size();
      }
   }
   return sn_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get data at index

   \param[in] orc_Index Index
   \param[in] osn_Role  Data role

   \return
   Data

   \created     09.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QVariant C_SdNdeDataPoolListDataSetModel::data(const QModelIndex & orc_Index, const sintn osn_Role) const
{
   QVariant c_Retval;

   if (orc_Index.isValid() == true)
   {
      const C_SdNdeDataPoolListDataSetModel::E_Rows e_Col = h_RowToEnum(orc_Index.row());
      if ((osn_Role == static_cast<sintn>(Qt::DisplayRole)) || (osn_Role == static_cast<sintn>(Qt::EditRole)))
      {
         if (orc_Index.row() >= 0)
         {
            const C_OSCNodeDataPoolList * const pc_OSCElement =
               C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(this->mu32_NodeIndex,
                                                                   this->mu32_DataPoolIndex,
                                                                   this->mu32_ListIndex);
            if (pc_OSCElement != NULL)
            {
               const uint32 u32_Index = static_cast<uint32>(orc_Index.column());
               if (u32_Index < pc_OSCElement->c_DataSets.size())
               {
                  const C_OSCNodeDataPoolDataSet & rc_DataSet = pc_OSCElement->c_DataSets[u32_Index];
                  switch (e_Col)
                  {
                  case eNAME:
                     c_Retval = QString(rc_DataSet.c_Name.c_str());
                     break;
                  case eCOMMENT:
                     c_Retval = QString(rc_DataSet.c_Comment.c_str());
                     break;
                  default:
                     break;
                  }
               }
            }
         }
      }
      else if (osn_Role == static_cast<sintn>(Qt::TextAlignmentRole))
      {
         switch (e_Col)
         {
         case eNAME:
            c_Retval = QVariant(Qt::AlignLeft | Qt::AlignVCenter);
            break;
         case eCOMMENT:
            c_Retval = QVariant(Qt::AlignLeft | Qt::AlignTop);
            break;
         default:
            break;
         }
      }
      else if (osn_Role == static_cast<sintn>(Qt::FontRole))
      {
         QFont c_Font;
         switch (e_Col)
         {
         case eNAME:
            c_Font = mc_STYLE_GUIDE_FONT_REGULAR_14;
            break;
         case eCOMMENT:
            c_Font = mc_STYLE_GUIDE_FONT_REGULAR_12;
            break;
         default:
            break;
         }
         //Convert point to pixel
         c_Font.setPixelSize(c_Font.pointSize());
         c_Retval = c_Font;
      }
      else if (osn_Role == static_cast<sintn>(Qt::ForegroundRole))
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
            if (this->data(orc_Index, msn_USER_ROLE_ERROR).toBool() == true)
            {
               c_Retval = mc_STYLE_GUIDE_COLOR_18;
            }
            break;
         default:
            c_Retval = mc_STYLE_GUIDE_COLOR_6;
            break;
         }
      }
      else if (osn_Role == msn_USER_ROLE_TOOL_TIP_HEADING)
      {
         switch (e_Col)
         {
         case eNAME:
            if (this->data(orc_Index, msn_USER_ROLE_ERROR).toBool() == true)
            {
               c_Retval = C_GtGetText::h_GetText("Dataset name");
            }
            break;
         default:
            c_Retval = false;
            break;
         }
      }
      else if (osn_Role == msn_USER_ROLE_TOOL_TIP_TYPE)
      {
         switch (e_Col)
         {
         case eNAME:
            if (this->data(orc_Index, msn_USER_ROLE_ERROR).toBool() == true)
            {
               c_Retval = static_cast<sintn>(C_NagToolTip::eERROR);
            }
            else
            {
               c_Retval = static_cast<sintn>(C_NagToolTip::eDEFAULT);
            }
            break;
         default:
            c_Retval = static_cast<sintn>(C_NagToolTip::eDEFAULT);
            break;
         }
      }
      else if (osn_Role == msn_USER_ROLE_TOOL_TIP_CONTENT)
      {
         switch (e_Col)
         {
         case eNAME:
            //Check error
            if (orc_Index.row() >= 0)
            {
               QString c_Output;
               const C_OSCNodeDataPoolList * const pc_OSCList = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(
                  this->mu32_NodeIndex,
                  this->mu32_DataPoolIndex,
                  this->mu32_ListIndex);
               if (pc_OSCList != NULL)
               {
                  bool q_NameInvalid = false;
                  bool q_NameConflict = false;
                  pc_OSCList->CheckErrorDataSet(static_cast<uint32>(orc_Index.column()), &q_NameConflict,
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
            break;
         default:
            c_Retval = false;
            break;
         }
      }
      else if (osn_Role == msn_USER_ROLE_ERROR)
      {
         switch (e_Col)
         {
         case eNAME:
            //Default
            c_Retval = false;
            //Check error
            if (orc_Index.row() >= 0)
            {
               bool q_NameInvalid = false;
               const C_OSCNodeDataPoolList * const pc_OSCList = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(
                  this->mu32_NodeIndex,
                  this->mu32_DataPoolIndex,
                  this->mu32_ListIndex);
               if (pc_OSCList != NULL)
               {
                  bool q_NameConflict = false;
                  pc_OSCList->CheckErrorDataSet(static_cast<uint32>(orc_Index.column()), &q_NameConflict,
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
            break;
         default:
            c_Retval = false;
            break;
         }
      }
      else
      {
         //No handling necessary
      }
   }
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set data at index

   \param[in] orc_Index Index
   \param[in] orc_Value New data
   \param[in] osn_Role  Data role

   \return
   true  success
   false failure

   \created     09.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_SdNdeDataPoolListDataSetModel::setData(const QModelIndex & orc_Index, const QVariant & orc_Value,
                                              const sintn osn_Role)
{
   bool q_Retval = false;

   if (data(orc_Index, osn_Role) != orc_Value)
   {
      if (osn_Role == static_cast<sintn>(Qt::EditRole))
      {
         if (orc_Index.column() >= 0)
         {
            const uint32 u32_Index = static_cast<uint32>(orc_Index.column());
            const C_SdNdeDataPoolListDataSetModel::E_Rows e_Row =
               h_RowToEnum(orc_Index.row());
            switch (e_Row)
            {
            case eNAME:
               Q_EMIT this->SigDataChange(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                          this->mu32_ListIndex,
                                          u32_Index, orc_Value,
                                          C_SdNdeDataPoolUtil::E_DataSetDataChangeType::eDATA_SET_NAME);
               q_Retval = true;
               break;
            case eCOMMENT:
               Q_EMIT this->SigDataChange(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                          this->mu32_ListIndex,
                                          u32_Index, orc_Value,
                                          C_SdNdeDataPoolUtil::E_DataSetDataChangeType::eDATA_SET_COMMENT);
               q_Retval = true;
               break;
            default:
               break;
            }

            //lint -e{1793} Qt example
            Q_EMIT dataChanged(orc_Index, orc_Index, QVector<stw_types::sintn>() << osn_Role);
         }
      }
   }
   return q_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get flags for item

   \param[in] orc_Index Item

   \return
   Flags for item

   \created     09.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
Qt::ItemFlags C_SdNdeDataPoolListDataSetModel::flags(const QModelIndex & orc_Index) const
{
   Qt::ItemFlags c_Retval;
   if (orc_Index.isValid() == true)
   {
      const C_SdNdeDataPoolListDataSetModel::E_Rows e_Row = h_RowToEnum(orc_Index.row());
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

//-----------------------------------------------------------------------------
/*!
   \brief   Get supported drop actions

   \return
   Supported drop actions

   \created     10.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
Qt::DropActions C_SdNdeDataPoolListDataSetModel::supportedDropActions(void) const
{
   return Qt::MoveAction;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Insert rows with content

   \param[in] orc_OSCInsertedElements Inserted elements OSC content
   \param[in] orc_UIInsertedElements  Inserted elements UI content
   \param[in] oru32_Row               Starting Row

   \created     10.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListDataSetModel::DoInsertColumns(const std::vector<C_OSCNodeDataPoolDataSet> & orc_OSCNames,
                                                      const std::vector<std::vector<C_OSCNodeDataPoolContent> > & orc_OSCDataSetValues,
                                                      const uint32 & oru32_Row)
{
   if (orc_OSCNames.size() == orc_OSCDataSetValues.size())
   {
      this->mc_OSCNames = orc_OSCNames;
      this->mc_OSCDataSetValues = orc_OSCDataSetValues;
      this->insertColumns(oru32_Row, orc_OSCNames.size());
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Insert items into model

   \param[in] osn_Row    Starting row
   \param[in] osn_Count  Number of inserted items
   \param[in] orc_Parent Parent

   \return
   true  success
   false failure

   \created     09.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_SdNdeDataPoolListDataSetModel::insertColumns(const sintn osn_Column, const sintn osn_Count,
                                                    const QModelIndex & orc_Parent)
{
   bool q_Retval = false;

   if ((osn_Count > 0) && (osn_Column >= 0))
   {
      if ((this->mc_OSCNames.size() == static_cast<uint32>(osn_Count)) &&
          (this->mc_OSCDataSetValues.size() == static_cast<uint32>(osn_Count)))
      {
         beginInsertColumns(orc_Parent, osn_Column, osn_Column + (osn_Count - 1));
         for (uint32 u32_Index = 0; u32_Index < static_cast<uint32>(osn_Count); ++u32_Index)
         {
            C_PuiSdHandler::h_GetInstance()->InsertDataPoolListDataSet(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                                       this->mu32_ListIndex,
                                                                       static_cast<uint32>(osn_Column) + u32_Index,
                                                                       this->mc_OSCNames[u32_Index],
                                                                       this->mc_OSCDataSetValues[u32_Index]);
         }
         endInsertColumns();
         Q_EMIT this->SigColumnCountChange(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                           this->mu32_ListIndex, this->columnCount());
         q_Retval =  true;
      }
   }
   return q_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Remove items from model

   \param[in] osn_Row    Starting row
   \param[in] osn_Count  Number of removed items
   \param[in] orc_Parent Parent

   \return
   true  success
   false failure

   \created     09.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_SdNdeDataPoolListDataSetModel::removeColumns(const sintn osn_Column, const sintn osn_Count,
                                                    const QModelIndex & orc_Parent)
{
   bool q_Retval = false;

   if ((osn_Count > 0) && (osn_Column >= 0))
   {
      beginRemoveColumns(orc_Parent, osn_Column, osn_Column + (osn_Count - 1));
      for (uint32 u32_Index = static_cast<uint32>(osn_Column + osn_Count); u32_Index > static_cast<uint32>(osn_Column);
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

//-----------------------------------------------------------------------------
/*!
   \brief   Move rows

   \param[in] orc_SourceParent      Source parent
   \param[in] osn_SourceRow         Source row
   \param[in] osn_Count             Count
   \param[in] orc_DestinationParent Destination parent
   \param[in] osn_DestinationChild  Destination child

   \return
   true  success
   false failure

   \created     09.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_SdNdeDataPoolListDataSetModel::moveColumns(const QModelIndex & orc_SourceParent, const sintn osn_SourceColumn,
                                                  const sintn osn_Count, const QModelIndex & orc_DestinationParent,
                                                  const sintn osn_DestinationChild)
{
   bool q_Retval = false;

   if (osn_Count > 0)
   {
      beginMoveColumns(orc_SourceParent, osn_SourceColumn, osn_Count, orc_DestinationParent, osn_DestinationChild);
      for (uint32 u32_It = 0; u32_It < static_cast<uint32>(osn_Count); ++u32_It)
      {
         C_PuiSdHandler::h_GetInstance()->MoveDataPoolListDataSet(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                                  this->mu32_ListIndex,
                                                                  static_cast<uint32>(osn_SourceColumn) + u32_It,
                                                                  static_cast<uint32>(osn_DestinationChild) + u32_It);
      }
      endMoveColumns();
      q_Retval = true;
   }
   return q_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Row to enum conversion

   \param[in]  ors32_Row Column

   \return
   Enum value

   \created     09.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdNdeDataPoolListDataSetModel::E_Rows C_SdNdeDataPoolListDataSetModel::h_RowToEnum(const sint32 & ors32_Row)
{
   C_SdNdeDataPoolListDataSetModel::E_Rows e_Retval = eNAME;
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

//-----------------------------------------------------------------------------
/*!
   \brief   Enum to column conversion

   \param[in] ore_Value Enum value

   \return
   Column
   -1 Error

   \created     20.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_SdNdeDataPoolListDataSetModel::h_EnumToRow(const C_SdNdeDataPoolListDataSetModel::E_Rows & ore_Value)
{
   sint32 s32_Retval;

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

//-----------------------------------------------------------------------------
/*!
   \brief   Mime types

   \return
   Table mime types

   \created     09.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QStringList C_SdNdeDataPoolListDataSetModel::mimeTypes(void) const
{
   //Append original types (Default events only accept this type)
   QStringList c_Retval = QAbstractTableModel::mimeTypes();

   //Append own types last (Default events only accept Qt types at start)
   c_Retval.append("application/stw/opensyde/system/node/data-pool/list/data-set/elements");
   c_Retval.append("application/stw/opensyde/system/node/data-pool/list/data-set/indices");

   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get mime data of specified indices

   \param[in] orc_Indices Indices

   \return
   Mime data of specified indices

   \created     09.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QMimeData * C_SdNdeDataPoolListDataSetModel::mimeData(const QModelIndexList & orc_Indices) const
{
   QMimeData * pc_Retval = NULL;

   if (orc_Indices.size() > 0)
   {
      if (this->mimeTypes().size() > 2)
      {
         const C_OSCNodeDataPoolList * const pc_OSCList = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(
            this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex);
         if (pc_OSCList != NULL)
         {
            std::vector<uint32> c_Cols = C_SdNdeDataPoolUtil::h_ConvertVector(orc_Indices, false);
            QString c_String;
            std::vector<C_OSCNodeDataPoolDataSet> c_OSCNames;
            std::vector<std::vector<C_OSCNodeDataPoolContent> > c_OSCDataSetValues;
            //Convert indices to unique rows
            C_Uti::h_Uniqueify(c_Cols);
            //Reserve
            c_OSCNames.reserve(c_Cols.size());
            c_OSCDataSetValues.reserve(c_Cols.size());
            //Copy relevant data
            for (uint32 u32_ItDataSetIndex = 0; u32_ItDataSetIndex < c_Cols.size(); ++u32_ItDataSetIndex)
            {
               std::vector<C_OSCNodeDataPoolContent> c_CurrentDataSetValues;
               c_OSCNames.push_back(pc_OSCList->c_DataSets.at(c_Cols[u32_ItDataSetIndex]));
               c_CurrentDataSetValues.reserve(pc_OSCList->c_Elements.size());
               for (uint32 u32_ItElementIndex = 0; u32_ItElementIndex < pc_OSCList->c_Elements.size();
                    ++u32_ItElementIndex)
               {
                  const C_OSCNodeDataPoolListElement & rc_Element = pc_OSCList->c_Elements[u32_ItElementIndex];
                  if (u32_ItDataSetIndex < rc_Element.c_DataSetValues.size())
                  {
                     c_CurrentDataSetValues.push_back(rc_Element.c_DataSetValues.at(c_Cols[u32_ItDataSetIndex]));
                  }
               }
               c_OSCDataSetValues.push_back(c_CurrentDataSetValues);
            }
            //Store in mime
            pc_Retval = new QMimeData();
            //Use default mime type so qt accepts these
            pc_Retval->setData(this->mimeTypes().at(0), "");
            C_SdClipBoardHelper::h_StoreDataPoolListDataSetsToString(c_OSCNames, c_OSCDataSetValues, c_String);
            pc_Retval->setData(this->mimeTypes().at(1), c_String.toStdString().c_str());
            C_SdClipBoardHelper::h_StoreIndicesToString(c_Cols, c_String);
            pc_Retval->setData(this->mimeTypes().at(2), c_String.toStdString().c_str());
         }
      }
   }
   return pc_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Signal model update

   \param[in] oru32_Row      Changed row
   \param[in] ore_ChangeType Change type

   \created     10.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListDataSetModel::HandleDataChange(const uint32 & oru32_Column,
                                                       const C_SdNdeDataPoolUtil::E_DataSetDataChangeType & ore_ChangeType)
{
   sint32 s32_Row = -1;

   switch (ore_ChangeType)
   {
   case C_SdNdeDataPoolUtil::eDATA_SET_NAME:
      s32_Row = C_SdNdeDataPoolListDataSetModel::h_EnumToRow(C_SdNdeDataPoolListDataSetModel::E_Rows::eNAME);
      Q_EMIT this->SigErrorChangePossible(this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex);
      break;
   case C_SdNdeDataPoolUtil::eDATA_SET_COMMENT:
      s32_Row = C_SdNdeDataPoolListDataSetModel::h_EnumToRow(C_SdNdeDataPoolListDataSetModel::E_Rows::eCOMMENT);
      break;
   default:
      //No handling
      break;
   }
   if (s32_Row >= 0)
   {
      Q_EMIT dataChanged(this->index(s32_Row, oru32_Column), this->index(s32_Row, oru32_Column));
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Register model data reset

   \created     18.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListDataSetModel::Reset(void)
{
   this->beginResetModel();
   this->endResetModel();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Register error change

   \created     30.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListDataSetModel::HandleErrorChange(void)
{
   Q_EMIT this->SigErrorChangePossible(this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check name

   \param[in] oru32_Index Data set index
   \param[in] orc_Name    Name

   \return
   true  Available
   false Already in use

   \created     07.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_SdNdeDataPoolListDataSetModel::CheckName(const uint32 & oru32_Index, const QString & orc_Name) const
{
   return C_PuiSdHandler::h_GetInstance()->CheckNodeDataPoolListDataSetNameAvailable(this->mu32_NodeIndex,
                                                                                     this->mu32_DataPoolIndex,
                                                                                     this->mu32_ListIndex,
                                                                                     orc_Name.toStdString().c_str(),
                                                                                     &oru32_Index);
}
