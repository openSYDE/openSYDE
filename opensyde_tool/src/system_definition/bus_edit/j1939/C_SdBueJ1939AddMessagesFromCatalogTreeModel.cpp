//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Short description

   Detailed description (optional). The module shall be described detailed if it is not described completely
   by short description.

   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QIcon>

#include "precomp_headers.hpp"
#include "stwerrors.hpp"
#include "constants.hpp"
#include "TglUtils.hpp"

#include "C_SdBueJ1939AddMessagesFromCatalogTreeModel.hpp"
#include "C_GtGetText.hpp"
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
/*! \brief  Default constructor
 *  \param[in]  opc_Parent      Pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueJ1939AddMessagesFromCatalogTreeModel::C_SdBueJ1939AddMessagesFromCatalogTreeModel(QObject * const opc_Parent) :
   C_TblTreeModelCheckable(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor

   Clean up.
*/
C_SdBueJ1939AddMessagesFromCatalogTreeModel::~C_SdBueJ1939AddMessagesFromCatalogTreeModel()
{
   this->m_CleanUpLastModel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Convey data changes to table
 *
 *  \param[in]     orc_MessagesImported          Messages extracted from the DBC file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939AddMessagesFromCatalogTreeModel::UpdateData(
   const std::vector<C_CieConverter::C_CieNodeMessage> & orc_MessagesImported)
{
   this->mc_MessagesImportedFromCatalog = orc_MessagesImported;

   beginResetModel();

   //Clear
   this->m_CleanUpLastModel();

   this->m_FillMessageInfo();

   endResetModel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Fill message info

 *
 *  Fills the message data in the list, to display in the overview table
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939AddMessagesFromCatalogTreeModel::m_FillMessageInfo()
{
   this->mpc_InvisibleRootItem = new C_TblTreeModelCheckableItem();

   this->mc_MessageInfoList.clear();
   this->mc_MessageInfoList.reserve(this->mc_MessagesImportedFromCatalog.size());

   // Add messages
   for (uint32_t u32_MessageIndex = 0; u32_MessageIndex < this->mc_MessagesImportedFromCatalog.size();
        u32_MessageIndex++)
   {
      C_CieConverter::C_CieNodeMessage & rc_Message =
         this->mc_MessagesImportedFromCatalog[u32_MessageIndex];

      C_MsgTableData c_MessageData;
      c_MessageData.c_Name = rc_Message.c_CanMessage.c_Name;
      c_MessageData.c_Comment = rc_Message.c_CanMessage.c_Comment;
      c_MessageData.c_CanId =  "0x" + QString::number(rc_Message.c_CanMessage.u32_CanId, 16).toUpper();

      //PGN
      C_OscCanUtilJ1939PgInfo c_PgInfo;
      C_OscCanUtil::h_GetJ1939PgInfoFromCanId(rc_Message.c_CanMessage.u32_CanId, c_PgInfo);
      c_MessageData.c_J1939Pgn = C_OscCanUtil::h_GetVisiblePgn(c_PgInfo.u32_Pgn);

      // Add message contents to tree
      C_TblTreeModelCheckableItem * const pc_MessageItem = new C_TblTreeModelCheckableItem();
      pc_MessageItem->c_Name = c_MessageData.c_J1939Pgn.toString();
      pc_MessageItem->u32_Index = u32_MessageIndex; // Message index in the vector
      pc_MessageItem->c_Icon = QIcon(":images/system_definition/IconMessage.svg");

      pc_MessageItem->q_CheckBoxVisible = true;
      pc_MessageItem->e_CheckState = Qt::Unchecked;

      pc_MessageItem->q_Selectable = true;
      pc_MessageItem->q_Enabled = true;

      // Add signals if any
      const uint32_t u32_SignalCount = rc_Message.c_CanMessage.c_Signals.size();
      if (u32_SignalCount > 0)
      {
         for (uint32_t u32_SignalIndex = 0; u32_SignalIndex < u32_SignalCount; u32_SignalIndex++)
         {
            C_SignalTableData c_SignalData;
            c_SignalData.u32_Spn = rc_Message.c_CanMessage.c_Signals.at(u32_SignalIndex).u32_J1939Spn;
            c_SignalData.c_Name = rc_Message.c_CanMessage.c_Signals.at(u32_SignalIndex).c_Element.c_Name;
            c_SignalData.c_Comment = rc_Message.c_CanMessage.c_Signals.at(u32_SignalIndex).c_Element.c_Comment;

            c_MessageData.c_Signals.push_back(c_SignalData);

            C_TblTreeModelCheckableItem * const pc_SignalItem = new C_TblTreeModelCheckableItem();
            pc_SignalItem->c_Name = QString::number(c_SignalData.u32_Spn);
            pc_SignalItem->u32_Index = u32_SignalIndex;
            pc_SignalItem->c_Icon = QIcon(":/images/system_definition/IconSignal.svg");

            pc_SignalItem->q_Enabled = true;
            pc_SignalItem->q_Selectable = true;

            pc_SignalItem->q_CheckBoxVisible = false;
            pc_SignalItem->e_CheckState = Qt::Unchecked;

            pc_MessageItem->AddChild(pc_SignalItem);
         }
      }
      this->mc_MessageInfoList.push_back(c_MessageData);
      this->mpc_InvisibleRootItem->AddChild(pc_MessageItem);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get tree column count

   \param[in]  orc_Parent  Parent

   \return
   Column count
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdBueJ1939AddMessagesFromCatalogTreeModel::columnCount(const QModelIndex & orc_Parent) const
{
   Q_UNUSED(orc_Parent)
   return 4;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get header data

   \param[in]  os32_Section      Section
   \param[in]  oe_Orientation    Orientation
   \param[in]  os32_Role         Role

   \return
   Header string
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdBueJ1939AddMessagesFromCatalogTreeModel::headerData(const int32_t os32_Section,
                                                                 const Qt::Orientation oe_Orientation,
                                                                 const int32_t os32_Role) const
{
   QVariant c_Retval = QAbstractItemModel::headerData(os32_Section, oe_Orientation, os32_Role);

   if (oe_Orientation == Qt::Orientation::Horizontal)
   {
      const C_SdBueJ1939AddMessagesFromCatalogTreeModel::E_Columns e_Col = h_ColumnToEnum(os32_Section);
      if (os32_Role == static_cast<int32_t>(Qt::DisplayRole))
      {
         switch (e_Col)
         {
         case eNAME:
            c_Retval = C_GtGetText::h_GetText("Name");
            break;
         case eCOMMENT:
            c_Retval = C_GtGetText::h_GetText("Comment");
            break;
         case ePGN_SPN:
            c_Retval = C_GtGetText::h_GetText("PGN/SPN");
            break;
         case eCAN_ID:
            c_Retval = C_GtGetText::h_GetText("CAN ID");
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
         case eNAME:
            c_Retval = C_GtGetText::h_GetText("Symbolic message name");
            break;
         case eCOMMENT:
            c_Retval = C_GtGetText::h_GetText("Comment for this message");
            break;
         case eCAN_ID:
            c_Retval = C_GtGetText::h_GetText("CAN Identifier(Hex). 29 Bit");
            break;
         case ePGN_SPN:
            c_Retval = C_GtGetText::h_GetText("Parameter Group Number. 18 Bit\nSuspect Parameter Number. 19 Bit");
            break;
         default:
            break;
         }
      }
      else if (os32_Role == ms32_USER_ROLE_TOOL_TIP_HEADING)
      {
         switch (e_Col)
         {
         case eNAME:
            c_Retval = C_GtGetText::h_GetText("Name");
            break;
         case eCOMMENT:
            c_Retval = C_GtGetText::h_GetText("Comment");
            break;
         case eCAN_ID:
            c_Retval = C_GtGetText::h_GetText("CAN Id");
            break;
         case ePGN_SPN:
            c_Retval = C_GtGetText::h_GetText("PGN/SPN");
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
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Decode index

   \param[in]   orc_ModelIndex      Model index
   \param[out]  oru32_MessageIndex  Message index
   \param[out]  oru32_SignalIndex   Signal index
   \param[in]   orq_IsSignal        Flag: is signal
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939AddMessagesFromCatalogTreeModel::m_DecodeIndex(const QModelIndex & orc_ModelIndex,
                                                                uint32_t & oru32_MessageIndex,
                                                                uint32_t & oru32_SignalIndex, bool & orq_IsSignal) const
{
   if (orc_ModelIndex.isValid())
   {
      //lint -e{9079} Result of Qt interface restrictions, set by index function
      const C_TblTreItem * const pc_TreeItem = static_cast<const C_TblTreItem *>(orc_ModelIndex.internalPointer());
      tgl_assert(pc_TreeItem != NULL);
      if (pc_TreeItem != NULL)
      {
         const QModelIndex c_ParentModelIndex = orc_ModelIndex.parent();
         if (c_ParentModelIndex.isValid())
         {
            //lint -e{9079} Result of Qt interface restrictions, set by index function
            const C_TblTreItem * const pc_ParentTreeItem =
               static_cast<const C_TblTreItem *>(c_ParentModelIndex.internalPointer());
            tgl_assert(pc_ParentTreeItem != NULL);
            if (pc_ParentTreeItem != NULL)
            {
               oru32_MessageIndex = pc_ParentTreeItem->u32_Index;
            }
            oru32_SignalIndex = pc_TreeItem->u32_Index;
            orq_IsSignal = true;
         }
         else
         {
            oru32_MessageIndex = orc_ModelIndex.row();
            oru32_SignalIndex = 0UL;
            orq_IsSignal = false;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get data at index

   \param[in]  orc_Index   Index
   \param[in]  os32_Role   Data role

   \return
   Data
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdBueJ1939AddMessagesFromCatalogTreeModel::data(const QModelIndex & orc_Index, const int32_t os32_Role) const
{
   QVariant c_Retval;

   // first columns are handled by C_TblTreItem
   if (orc_Index.column() == 0)
   {
      c_Retval = C_TblTreeModelCheckable::data(orc_Index, os32_Role);
   }
   else
   {
      if (orc_Index.isValid() == true)
      {
         const C_SdBueJ1939AddMessagesFromCatalogTreeModel::E_Columns e_Col = h_ColumnToEnum(orc_Index.column());
         if ((os32_Role == static_cast<int32_t>(Qt::DisplayRole)) || (os32_Role == static_cast<int32_t>(Qt::EditRole)))
         {
            const int32_t s32_Index = orc_Index.row();

            if (s32_Index >= 0)
            {
               uint32_t u32_MessageIndex = 0UL;
               uint32_t u32_SignalIndex = 0UL;
               bool q_IsSignal = false;

               // QModelIndex (orc_Index) could be of main tree item (message) or sub tree item (signal).
               // Analyze the index and accordingly fetch the item to be displayed.
               this->m_DecodeIndex(orc_Index, u32_MessageIndex, u32_SignalIndex, q_IsSignal);
               if (q_IsSignal == false)
               {
                  switch (e_Col)
                  {
                  case eCAN_ID:
                     c_Retval = this->mc_MessageInfoList[u32_MessageIndex].c_CanId;
                     break;
                  case eNAME:
                     c_Retval = this->mc_MessageInfoList[u32_MessageIndex].c_Name.c_str();
                     break;
                  case eCOMMENT:
                     c_Retval = this->mc_MessageInfoList[u32_MessageIndex].c_Comment.c_str();
                     break;
                  case ePGN_SPN:
                  // PGN / SPN handled by tree item name
                  default:
                     break;
                  }
               }
               else
               {
                  switch (e_Col)
                  {
                  case eNAME:
                     c_Retval = this->mc_MessageInfoList[u32_MessageIndex].c_Signals[u32_SignalIndex].c_Name.c_str();
                     break;
                  case eCOMMENT:
                     c_Retval = this->mc_MessageInfoList[u32_MessageIndex].c_Signals[u32_SignalIndex].c_Comment.c_str();
                     break;
                  case eCAN_ID:
                  // not applicable for signals
                  case ePGN_SPN:
                  // PGN / SPN handled by tree item name
                  default:
                     break;
                  }
               }
            }
         }

         else
         {
            //Unknown
         }
      }
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
C_SdBueJ1939AddMessagesFromCatalogTreeModel::E_Columns C_SdBueJ1939AddMessagesFromCatalogTreeModel::h_ColumnToEnum(
   const int32_t & ors32_Column)
{
   C_SdBueJ1939AddMessagesFromCatalogTreeModel::E_Columns e_Retval = eNAME;
   switch (ors32_Column)
   {
   case 0:
      e_Retval = ePGN_SPN;
      break;
   case 1:
      e_Retval = eCAN_ID;
      break;
   case 2:
      e_Retval = eNAME;
      break;
   case 3:
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
int32_t C_SdBueJ1939AddMessagesFromCatalogTreeModel::h_EnumToColumn(
   const C_SdBueJ1939AddMessagesFromCatalogTreeModel::E_Columns & ore_Value)
{
   int32_t s32_Retval;

   switch (ore_Value)
   {
   case ePGN_SPN:
      s32_Retval = 0;
      break;
   case eCAN_ID:
      s32_Retval = 1;
      break;
   case eNAME:
      s32_Retval = 2;
      break;
   case eCOMMENT:
      s32_Retval = 3;
      break;
   default:
      s32_Retval = -1;
      break;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize

   \param[in]  orc_ElementIndices   Element indices

   \return
   STW error codes

   \retval   C_NO_ERR   Init successful
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdBueJ1939AddMessagesFromCatalogTreeModel::Init(const std::vector<uint32_t> & orc_ElementIndices)
{
   Q_UNUSED(orc_ElementIndices)
   return C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get checked items

\param[in,out]  orc_ElementIndices           Element indices
\param[in,out]  orc_ChildIndicesPerElement   Child indices per element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939AddMessagesFromCatalogTreeModel::GetCheckedItems(std::vector<uint32_t> & orc_ElementIndices, std::vector<std::vector<uint32_t> > &
                                                                  orc_ChildIndicesPerElement)
const
{
   if (this->mpc_InvisibleRootItem != NULL)
   {
      std::vector<C_TblTreSimpleItem *> c_InvisibleRootChildren = this->mpc_InvisibleRootItem->c_Children;
      orc_ElementIndices.clear();
      orc_ChildIndicesPerElement.clear();

      // Fetch the message items that have the "CheckState" as "Checked"
      for (uint32_t u32_MessageItem = 0;
           u32_MessageItem < c_InvisibleRootChildren.size(); u32_MessageItem++)
      {
         C_TblTreeModelCheckableItem * const pc_MessageItem =
            dynamic_cast<C_TblTreeModelCheckableItem *>(c_InvisibleRootChildren.at(u32_MessageItem));
         if ((pc_MessageItem != NULL) && (pc_MessageItem->e_CheckState == Qt::Checked))
         {
            orc_ElementIndices.push_back(pc_MessageItem->u32_Index);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get checked item count

   \return
   Checked item count
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SdBueJ1939AddMessagesFromCatalogTreeModel::GetCheckedItemCount() const
{
   std::vector<uint32_t> c_MessageIndices;
   std::vector<std::vector<uint32_t> > c_SignalIndices;

   // Only message items are checkable. Hence fetches all checked messages.
   this->GetCheckedItems(c_MessageIndices, c_SignalIndices);

   return c_MessageIndices.size();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Select all the parent items
 *
      Select all messages
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939AddMessagesFromCatalogTreeModel::SelectAllParentItems()
{
   if (this->mpc_InvisibleRootItem != NULL)
   {
      std::vector<C_TblTreSimpleItem *> c_InvisibleRootChildren = this->mpc_InvisibleRootItem->c_Children;
      for (uint32_t u32_MessageItem = 0;
           u32_MessageItem < c_InvisibleRootChildren.size(); u32_MessageItem++)
      {
         C_TblTreeModelCheckableItem * const pc_MessageItem =
            dynamic_cast<C_TblTreeModelCheckableItem *>(c_InvisibleRootChildren.at(u32_MessageItem));
         pc_MessageItem->e_CheckState = Qt::Checked;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Select only the filtered messages

   \param[in]       orc_FilteredItems     Filtered messages
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939AddMessagesFromCatalogTreeModel::SelectFilteredParentItems(const QStringList & orc_FilteredItems)
{
   if (this->mpc_InvisibleRootItem != NULL)
   {
      std::vector<C_TblTreSimpleItem *> c_InvisibleRootChildren = this->mpc_InvisibleRootItem->c_Children;
      for (uint32_t u32_MessageItem = 0;
           u32_MessageItem < c_InvisibleRootChildren.size(); u32_MessageItem++)
      {
         C_TblTreeModelCheckableItem * const pc_MessageItem =
            dynamic_cast<C_TblTreeModelCheckableItem *>(c_InvisibleRootChildren.at(u32_MessageItem));

         // Search the filtered message PGN in the entire list of messages and enable its checkbox
         if (orc_FilteredItems.contains(pc_MessageItem->c_Name))
         {
            pc_MessageItem->e_CheckState = Qt::Checked;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Unselect all the parent items
 *
      Unselect all messages
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939AddMessagesFromCatalogTreeModel::UnselectAllParentItems()
{
   if (this->mpc_InvisibleRootItem != NULL)
   {
      std::vector<C_TblTreSimpleItem *> c_InvisibleRootChildren = this->mpc_InvisibleRootItem->c_Children;
      for (uint32_t u32_MessageItem = 0;
           u32_MessageItem < c_InvisibleRootChildren.size(); u32_MessageItem++)
      {
         C_TblTreeModelCheckableItem * const pc_MessageItem =
            dynamic_cast<C_TblTreeModelCheckableItem *>(c_InvisibleRootChildren.at(u32_MessageItem));
         pc_MessageItem->e_CheckState = Qt::Unchecked;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get the selected messages

   \param[in,out]   orc_SelectedMessages   Selected messages

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939AddMessagesFromCatalogTreeModel::GetSelectedMessages(
   std::vector<C_CieConverter::C_CieNodeMessage> & orc_SelectedMessages) const
{
   std::vector<uint32_t> c_MessageIndices;
   std::vector<std::vector<uint32_t> > c_SignalIndices;

   // Only message items are checkable. Hence fetches all checked message indices.
   this->GetCheckedItems(c_MessageIndices, c_SignalIndices);

   for (uint32_t u32_MessageIndex = 0; u32_MessageIndex < static_cast<uint32_t>(c_MessageIndices.size());
        u32_MessageIndex++)
   {
      const uint32_t u32_SelectedMessageIndex = c_MessageIndices.at(u32_MessageIndex);
      if (u32_SelectedMessageIndex < this->mc_MessagesImportedFromCatalog.size())
      {
         const C_CieConverter::C_CieNodeMessage & rpc_Message = this->mc_MessagesImportedFromCatalog.at(
            u32_SelectedMessageIndex);
         orc_SelectedMessages.push_back(rpc_Message);
      }
   }
}
