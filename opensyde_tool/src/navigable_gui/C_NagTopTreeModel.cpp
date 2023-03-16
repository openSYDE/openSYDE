//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Model for topology item navigation tree (implementation)

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "TglUtils.hpp"
#include "constants.hpp"
#include "C_GtGetText.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_SdUtil.hpp"
#include "C_NagToolTip.hpp"
#include "C_TblTreItem.hpp"

#include "C_NagTopTreeModel.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   \param[in,out]  opc_Parent    Parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_NagTopTreeModel::C_NagTopTreeModel(QObject * const opc_Parent) :
   C_TblTreModel(opc_Parent),
   mq_TypeNode(false),
   mc_ErrorIcon(C_SdUtil::h_InitStaticIconSvg("://images/Error_iconV2.svg", mc_ICON_SIZE_24))
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor.

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_NagTopTreeModel::~C_NagTopTreeModel()
{
   delete (this->mpc_InvisibleRootItem);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Return number of columns.

   Here: Only one column.

   \param[in]  orc_Parent  Parent

   \return
   Number of columns, namely 1
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_NagTopTreeModel::columnCount(const QModelIndex & orc_Parent) const
{
   Q_UNUSED(orc_Parent)
   return 1;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overridden data method for getting data at index

   Here: handle tool tip error type

   \param[in]  orc_Index   Index
   \param[in]  os32_Role    Data role

   \return
   Data
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_NagTopTreeModel::data(const QModelIndex & orc_Index, const int32_t os32_Role) const
{
   QVariant c_Retval = C_TblTreModel::data(orc_Index, os32_Role);

   if ((orc_Index.isValid() == true) && (orc_Index.column() == 0))
   {
      if (os32_Role == ms32_USER_ROLE_TOOL_TIP_TYPE)
      {
         // if there is a tooltip, it is of type error (no non-error tooltips)
         c_Retval = static_cast<int32_t>(C_NagToolTip::eERROR);
      }
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set item types are nodes flag

   \param[in]  oq_IsNode   Flag if items are nodes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagTopTreeModel::SetTypeNode(const bool oq_IsNode)
{
   this->mq_TypeNode = oq_IsNode;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set tree content

   \param[in]  orc_Content       List of content names
   \param[in]  orc_SubContent    List of sub content names (corresponding to list of content names; or empty if none)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagTopTreeModel::SetContent(const std::vector<QString> & orc_Content,
                                   const std::vector<std::vector<QString> > & orc_SubContent)
{
   uint32_t u32_FlatIndex = 0;

   if (orc_SubContent.empty() == false)
   {
      tgl_assert(orc_Content.size() == orc_SubContent.size());
   }

   // clean up
   this->Clear();

   // fill with data
   this->beginResetModel();
   if (this->mpc_InvisibleRootItem == NULL)
   {
      this->mpc_InvisibleRootItem = new C_TblTreItem();
   }

   // collect items
   for (uint32_t u32_ContIt = 0; u32_ContIt < orc_Content.size(); u32_ContIt++)
   {
      C_TblTreItem * const pc_Parent = new C_TblTreItem();
      pc_Parent->c_Name = orc_Content[u32_ContIt];
      pc_Parent->u32_Index = u32_FlatIndex;
      pc_Parent->c_Icon = this->m_GetPlaceholderIcon();

      // collect subitems
      const std::vector<QString> & rc_CurSubContent =
         orc_SubContent.empty() ? std::vector<QString>() : orc_SubContent[u32_ContIt];
      for (uint32_t u32_SubIt = 0; u32_SubIt < rc_CurSubContent.size(); u32_SubIt++)
      {
         // create child
         C_TblTreItem * const pc_Child = new C_TblTreItem();
         pc_Child->c_Name = rc_CurSubContent[u32_SubIt];
         pc_Child->u32_Index = u32_FlatIndex;
         pc_Child->c_Icon = this->m_GetPlaceholderIcon();

         // add to tree
         pc_Parent->AddChild(pc_Child);

         // count up flat index
         u32_FlatIndex++;
      }

      // disable parent if it has sub nodes and do not add to flat count in this case
      if (pc_Parent->c_Children.empty() == false)
      {
         pc_Parent->q_Enabled = false;
         pc_Parent->q_Selectable = false;
      }
      else
      {
         u32_FlatIndex++;
      }

      // add to tree
      this->mpc_InvisibleRootItem->AddChild(pc_Parent);
   }

   this->endResetModel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set list errors

   \param[in]  orc_Error   List error states
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagTopTreeModel::SetError(const std::vector<bool> & orc_Error)
{
   if (this->mpc_InvisibleRootItem != NULL)
   {
      std::vector<C_TblTreSimpleItem *>::const_iterator c_ItParents;
      for (c_ItParents = this->mpc_InvisibleRootItem->c_Children.begin();
           c_ItParents != this->mpc_InvisibleRootItem->c_Children.end(); ++c_ItParents)
      {
         C_TblTreItem * const pc_ParentItem = dynamic_cast<C_TblTreItem *>(*c_ItParents);

         if (pc_ParentItem != NULL)
         {
            if (pc_ParentItem->c_Children.empty() == true)
            {
               // flat case
               if (pc_ParentItem->u32_Index < orc_Error.size())
               {
                  this->m_SetChildError(pc_ParentItem, orc_Error[pc_ParentItem->u32_Index]);
               }
            }
            else
            {
               // sub item case
               QStringList c_ChildNames;
               std::vector<C_TblTreSimpleItem *>::const_iterator c_ItChildren;
               for (c_ItChildren = pc_ParentItem->c_Children.begin();
                    c_ItChildren != pc_ParentItem->c_Children.end(); ++c_ItChildren)
               {
                  C_TblTreItem * const pc_ChildItem = dynamic_cast<C_TblTreItem *>(*c_ItChildren);

                  if (pc_ChildItem != NULL)
                  {
                     if (pc_ChildItem->u32_Index < orc_Error.size())
                     {
                        this->m_SetChildError(pc_ChildItem, orc_Error[pc_ChildItem->u32_Index]);
                        if (orc_Error[pc_ChildItem->u32_Index] == true)
                        {
                           c_ChildNames.append(pc_ChildItem->c_Name);
                        }
                     }
                  }
               }

               // set error on parent if a child is erroneous
               this->m_SetParentError(pc_ParentItem, !c_ChildNames.empty(), c_ChildNames.join("; "));
            }
         }
      }
   }

   // reload tree
   Q_EMIT (this->dataChanged(this->index(0, 0), this->index(this->rowCount() - 1, 0), // flat tree so column is always 0
                             QVector<int32_t>() << static_cast<int32_t>(Qt::DecorationRole)));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update text of a single item

   If item index corresponds to a child, the content is set as parent name (child names can not change).

   \param[in]  ou32_Index     Item index
   \param[in]  orc_Content    Content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagTopTreeModel::UpdateItem(const uint32_t ou32_Index, const QString & orc_Content)
{
   if (this->mpc_InvisibleRootItem != NULL)
   {
      std::vector<C_TblTreSimpleItem *>::const_iterator c_ItParents;
      for (c_ItParents = this->mpc_InvisibleRootItem->c_Children.begin();
           c_ItParents != this->mpc_InvisibleRootItem->c_Children.end(); ++c_ItParents)
      {
         C_TblTreItem * const pc_ParentItem = dynamic_cast<C_TblTreItem *>(*c_ItParents);

         if (pc_ParentItem != NULL)
         {
            if (pc_ParentItem->c_Children.empty() == true)
            {
               // flat case
               if (pc_ParentItem->u32_Index == ou32_Index)
               {
                  pc_ParentItem->c_Name = orc_Content;
                  break;
               }
            }
            else
            {
               // sub item case: search for child that corresponds to index
               std::vector<C_TblTreSimpleItem *>::const_iterator c_ItChildren;
               for (c_ItChildren = pc_ParentItem->c_Children.begin();
                    c_ItChildren != pc_ParentItem->c_Children.end(); ++c_ItChildren)
               {
                  C_TblTreItem * const pc_ChildItem = dynamic_cast<C_TblTreItem *>(*c_ItChildren);

                  if (pc_ChildItem != NULL)
                  {
                     if (pc_ChildItem->u32_Index == ou32_Index)
                     {
                        pc_ParentItem->c_Name = orc_Content;
                        break;
                     }
                  }
               }
            }
         }
      }
   }

   // skip the data changed signal here, because this will anyways be triggered by error check afterwards
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Clear all children
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagTopTreeModel::Clear(void)
{
   if (this->mpc_InvisibleRootItem != NULL)
   {
      std::vector<C_TblTreSimpleItem *>::const_iterator c_ItChildren;

      this->beginResetModel();

      // clear subitems (children of children)
      for (c_ItChildren = this->mpc_InvisibleRootItem->c_Children.begin();
           c_ItChildren != this->mpc_InvisibleRootItem->c_Children.end(); ++c_ItChildren)
      {
         if (*c_ItChildren != NULL)
         {
            (*c_ItChildren)->ClearChildren();
         }
      }

      // clear items (children)
      this->mpc_InvisibleRootItem->ClearChildren();
      this->endResetModel();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get flat index from model index

   Parents with children have the same index as their first child.

   Example:
                  flat index
      Parent A       0
      Parent B       1
         Child A     1
         Child B     2
      Parent C       3
      Parent D       4

   \param[in]  orc_ModelIndex    Model index

   \return
   flat row index
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_NagTopTreeModel::h_GetFlatIndexFromModelIndex(const QModelIndex & orc_ModelIndex)
{
   uint32_t u32_Return = 0;

   //lint -e{9079}  Result of Qt interface restrictions, set by index function
   const C_TblTreItem * const pc_TreeItem = static_cast<const C_TblTreItem *>(orc_ModelIndex.internalPointer());

   if (pc_TreeItem != NULL)
   {
      u32_Return = pc_TreeItem->u32_Index;
   }

   return u32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get model index from flat index

   Counterpart of C_NagTopTreeModel::h_GetFlatIndexFromModelIndex -> see this for details.

   \param[in]  ou32_FlatIndex    Flat index

   \return
   Model index
*/
//----------------------------------------------------------------------------------------------------------------------
QModelIndex C_NagTopTreeModel::GetModelIndexFromFlatIndex(const uint32_t ou32_FlatIndex)
{
   QModelIndex c_ModelIndex;
   bool q_Found = false;

   for (uint32_t u32_ItParents = 0;
        (u32_ItParents < this->mpc_InvisibleRootItem->c_Children.size()) && (q_Found == false); u32_ItParents++)
   {
      C_TblTreItem * const pc_ParentItem =
         dynamic_cast<C_TblTreItem *>(this->mpc_InvisibleRootItem->c_Children[u32_ItParents]);

      if (pc_ParentItem != NULL)
      {
         if (pc_ParentItem->c_Children.empty() == true)
         {
            if (pc_ParentItem->u32_Index == ou32_FlatIndex)
            {
               c_ModelIndex = this->index(u32_ItParents, 0);
               q_Found = true;
            }
         }
         else
         {
            for (uint32_t u32_ItChildren = 0; u32_ItChildren < pc_ParentItem->c_Children.size(); u32_ItChildren++)
            {
               C_TblTreItem * const pc_ChildItem =
                  dynamic_cast<C_TblTreItem *>(pc_ParentItem->c_Children[u32_ItChildren]);
               if ((pc_ChildItem != NULL) && (pc_ChildItem->u32_Index == ou32_FlatIndex))
               {
                  c_ModelIndex = this->index(u32_ItChildren, 0, this->index(u32_ItParents, 0));
                  q_Found = true;
                  break;
               }
            }
         }
      }
   }

   return c_ModelIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set child error including tool tip

   \param[in,out]  opc_Item   Item
   \param[in]      oq_Error   true: error; false: no error
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagTopTreeModel::m_SetChildError(C_TblTreItem * const opc_Item, const bool oq_Error)
{
   QString c_ToopTipContent;

   if (this->mq_TypeNode == true)
   {
      bool q_Error;
      C_SdUtil::h_GetErrorToolTipNode(opc_Item->u32_Index, c_ToopTipContent, q_Error, false);
   }
   else
   {
      C_SdUtil::h_GetErrorToolTipBus(opc_Item->u32_Index, c_ToopTipContent);
   }

   this->m_SetItemError(opc_Item, oq_Error, c_ToopTipContent);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set parent error containing information which child has a problem

   \param[in,out]  opc_Item         Tree item
   \param[in]      oq_Error         true: error; false: no error
   \param[in]      orc_ChildNames   Child names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagTopTreeModel::m_SetParentError(C_TblTreItem * const opc_Item, const bool oq_Error,
                                         const QString & orc_ChildNames)
{
   this->m_SetItemError(opc_Item, oq_Error,
                        C_GtGetText::h_GetText("Following items contain invalid content:\n") + orc_ChildNames);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set or remove error on specific tree item

   \param[in,out]  opc_Item            Tree item
   \param[in]      oq_Error            true: error; false: no errors
   \param[in]      orc_ToolTipContent  Tool tip content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagTopTreeModel::m_SetItemError(C_TblTreItem * const opc_Item, const bool oq_Error,
                                       const QString & orc_ToolTipContent)
{
   if (oq_Error == true)
   {
      if (this->mq_TypeNode == true)
      {
         opc_Item->c_ToolTipHeading = C_GtGetText::h_GetText("Invalid Node Content");
      }
      else
      {
         opc_Item->c_ToolTipHeading = C_GtGetText::h_GetText("Invalid Bus Content");
      }
      opc_Item->c_ToolTipContent = orc_ToolTipContent;
      opc_Item->c_Icon = this->mc_ErrorIcon;
   }
   else
   {
      opc_Item->c_ToolTipHeading = "";
      opc_Item->c_ToolTipContent = "";
      opc_Item->c_Icon = m_GetPlaceholderIcon();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get placeholder icon

   \return
   transparent icon
*/
//----------------------------------------------------------------------------------------------------------------------
QIcon C_NagTopTreeModel::m_GetPlaceholderIcon(void)
{
   QPixmap c_Pixmap(mc_ICON_SIZE_24);

   c_Pixmap.fill(Qt::transparent);

   return static_cast<QIcon>(c_Pixmap);
}
