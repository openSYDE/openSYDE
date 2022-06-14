//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree model for code generation items

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwerrors.h"
#include "TGLUtils.h"

#include "C_GtGetText.h"
//#include "C_SdCodeGenerationModel.h"
#include "C_TblTreeModelCheckable.h"
#include "C_PuiSdHandler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_TblTreeModelCheckableItem::C_TblTreeModelCheckableItem(void) :
   C_TblTreItem(),
   e_CheckState(Qt::Unchecked),
   q_CheckBoxVisible(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Set up model

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_TblTreeModelCheckable::C_TblTreeModelCheckable(QObject * const opc_Parent) :
   C_TblTreModel(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_TblTreeModelCheckable::~C_TblTreeModelCheckable(void)
{
   this->m_CleanUpLastModel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get tree column count

   \param[in]  orc_Parent  Parent

   \return
   Column count
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_TblTreeModelCheckable::columnCount(const QModelIndex & orc_Parent) const
{
   Q_UNUSED(orc_Parent)
   return 1;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get data at index

   \param[in]  orc_Index   Index
   \param[in]  osn_Role    Data role

   \return
   Data
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_TblTreeModelCheckable::data(const QModelIndex & orc_Index, const sintn osn_Role) const
{
   QVariant c_Return;

   if ((osn_Role == static_cast<sintn>(Qt::CheckStateRole)) &&
       (orc_Index.isValid() == true))
   {
      //lint -e{9079}  Result of Qt interface restrictions, set by index function
      C_TblTreeModelCheckableItem * const pc_TreeItem =
         static_cast<C_TblTreeModelCheckableItem *>(orc_Index.internalPointer());

      if ((pc_TreeItem != NULL) && (pc_TreeItem->q_CheckBoxVisible))
      {
         c_Return = static_cast<sintn>(pc_TreeItem->e_CheckState);
      }
   }
   else
   {
      c_Return = C_TblTreModel::data(orc_Index, osn_Role);
   }

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set data at index

   \param[in]  orc_Index   Index
   \param[in]  orc_Value   New data
   \param[in]  osn_Role    Data role

   \return
   true  success
   false failure
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_TblTreeModelCheckable::setData(const QModelIndex & orc_Index, const QVariant & orc_Value, const sintn osn_Role)
{
   bool q_Return = false;

   if (osn_Role == static_cast<sintn>(Qt::CheckStateRole))
   {
      if (orc_Index.isValid() == true)
      {
         //lint -e{9079}  Result of Qt interface restrictions, set by index function
         C_TblTreeModelCheckableItem * const pc_TreeItem =
            static_cast<C_TblTreeModelCheckableItem *>(orc_Index.internalPointer());

         if (pc_TreeItem != NULL)
         {
            QModelIndex c_StartIndex = orc_Index;
            QModelIndex c_EndIndex = orc_Index;

            // Switching from unchecked and partial to checked
            pc_TreeItem->e_CheckState = (pc_TreeItem->e_CheckState == Qt::Checked) ? Qt::Unchecked : Qt::Checked;

            // Check if the item has children
            this->m_CheckChildren(pc_TreeItem, c_StartIndex, c_EndIndex);

            // or has a parent
            this->m_CheckParent(pc_TreeItem, orc_Index, c_StartIndex);

            Q_EMIT (this->dataChanged(c_StartIndex, c_EndIndex, QVector<stw_types::sintn>() << osn_Role));
            q_Return = true;
         }
      }
   }
   else
   {
      q_Return = C_TblTreModel::setData(orc_Index, orc_Value, osn_Role);
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get item flags

   \param[in]  orc_Index   Index

   \return
   Item flags
*/
//----------------------------------------------------------------------------------------------------------------------
Qt::ItemFlags C_TblTreeModelCheckable::flags(const QModelIndex & orc_Index) const
{
   Qt::ItemFlags c_Retval = C_TblTreModel::flags(orc_Index);

   if (orc_Index.isValid() == true)
   {
      //lint -e{9079}  Result of Qt interface restrictions, set by index function
      const C_TblTreeModelCheckableItem * const pc_TreeItem =
         static_cast<const C_TblTreeModelCheckableItem *>(orc_Index.internalPointer());
      if (pc_TreeItem != NULL)
      {
         if (pc_TreeItem->q_CheckBoxVisible == true)
         {
            c_Retval = c_Retval | Qt::ItemIsUserCheckable;
         }
      }
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks all children (recursive) and adapts the check state to the state of the parent

   \param[in,out]  opc_TreeItem     Parent to check its children
   \param[in]      orc_ItemIndex    Index of the parent
   \param[out]     orc_EndIndex     Last index of the last child
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblTreeModelCheckable::m_CheckChildren(C_TblTreeModelCheckableItem * const opc_TreeItem,
                                              const QModelIndex & orc_ItemIndex, QModelIndex & orc_EndIndex)
{
   if (opc_TreeItem->c_Children.size() > 0)
   {
      uint32 u32_ChildCounter;
      const uint32 u32_IndexLastChild = static_cast<uint32>(opc_TreeItem->c_Children.size() - 1UL);

      orc_EndIndex = this->index(u32_IndexLastChild, 0, orc_ItemIndex);

      for (u32_ChildCounter = 0U; u32_ChildCounter < opc_TreeItem->c_Children.size(); ++u32_ChildCounter)
      {
         C_TblTreeModelCheckableItem * const pc_ChildItem =
            dynamic_cast<C_TblTreeModelCheckableItem * const>(opc_TreeItem->c_Children[u32_ChildCounter]);
         const QModelIndex c_ChildIndex = this->index(u32_ChildCounter, 0, orc_ItemIndex);

         // Set all children to the same checked state
         if (pc_ChildItem != NULL)
         {
            if (pc_ChildItem->q_Enabled == true)
            {
               pc_ChildItem->e_CheckState = opc_TreeItem->e_CheckState;

               // Check the children for their children
               this->m_CheckChildren(pc_ChildItem, c_ChildIndex, orc_EndIndex);
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks the parent (recursive) and adapts the check state to the state of the child

   \param[in]   opc_TreeItem     Parent to check its children
   \param[in]   orc_ItemIndex    Index of the child
   \param[out]  orc_StartIndex   First index of the first parent
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblTreeModelCheckable::m_CheckParent(const C_TblTreeModelCheckableItem * const opc_TreeItem,
                                            const QModelIndex & orc_ItemIndex, QModelIndex & orc_StartIndex)
{
   if (opc_TreeItem->pc_Parent != NULL)
   {
      C_TblTreeModelCheckableItem * const pc_ParentItem =
         dynamic_cast<C_TblTreeModelCheckableItem * const>(opc_TreeItem->pc_Parent);

      if (pc_ParentItem != NULL)
      {
         bool q_AllAreChecked = true;
         bool q_AtLeastOneIsChecked = false;
         uint32 u32_ChildCounter;
         const QModelIndex c_ParentIndex = this->parent(orc_ItemIndex);
         orc_StartIndex = c_ParentIndex;

         // Check the other children to get the necessary state of the parent
         for (u32_ChildCounter = 0U; u32_ChildCounter < pc_ParentItem->c_Children.size(); ++u32_ChildCounter)
         {
            C_TblTreeModelCheckableItem * const pc_ChildItem =
               dynamic_cast<C_TblTreeModelCheckableItem * const>(pc_ParentItem->c_Children[u32_ChildCounter]);

            if (pc_ChildItem != NULL)
            {
               if (pc_ChildItem->e_CheckState == Qt::Unchecked)
               {
                  q_AllAreChecked = false;
               }
               else
               {
                  q_AtLeastOneIsChecked = true;
               }
            }
         }

         if (q_AllAreChecked == true)
         {
            pc_ParentItem->e_CheckState = Qt::Checked;
         }
         else if (q_AtLeastOneIsChecked == true)
         {
            pc_ParentItem->e_CheckState = Qt::PartiallyChecked;
         }
         else
         {
            pc_ParentItem->e_CheckState = Qt::Unchecked;
         }

         // Check the parent of the parent
         this->m_CheckParent(pc_ParentItem, c_ParentIndex, orc_StartIndex);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Clean up current model

   Warning: pointer might be invalid after call of this function
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblTreeModelCheckable::m_CleanUpLastModel(void)
{
   if (this->mpc_InvisibleRootItem != NULL)
   {
      delete (this->mpc_InvisibleRootItem);
      this->mpc_InvisibleRootItem = NULL;
   }
}
