//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree model base class (implementation)

   Tree model base class

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "TGLUtils.h"
#include "stwtypes.h"
#include "constants.h"
#include "C_TblTreModel.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;

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
C_TblTreModel::C_TblTreModel(QObject * const opc_Parent) :
   QAbstractItemModel(opc_Parent),
   mpc_InvisibleRootItem(NULL)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get data index

   \param[in] osn_Row    Row
   \param[in] osn_Column Column
   \param[in] orc_Parent Parent

   \return
   Data index (may be invalid = invalid parameters)
*/
//----------------------------------------------------------------------------------------------------------------------
QModelIndex C_TblTreModel::index(const sintn osn_Row, const sintn osn_Column, const QModelIndex & orc_Parent) const
{
   QModelIndex c_Retval;

   if (hasIndex(osn_Row, osn_Column, orc_Parent) == true)
   {
      if (orc_Parent.isValid() == true)
      {
         //lint -e{925}  Result of Qt interface restrictions, set by index function
         const C_TblTreItem * const pc_TreeItem =
            static_cast<const C_TblTreItem * const>(orc_Parent.internalPointer());
         if (pc_TreeItem != NULL)
         {
            c_Retval =
               this->createIndex(osn_Row, osn_Column, pc_TreeItem->c_Children.at(static_cast<uint32>(osn_Row)));
         }
      }
      else
      {
         if (this->mpc_InvisibleRootItem != NULL)
         {
            //Top level
            c_Retval =
               this->createIndex(osn_Row, osn_Column,
                                 this->mpc_InvisibleRootItem->c_Children.at(static_cast<uint32>(osn_Row)));
         }
      }
   }
   return c_Retval;
   //lint -e{1763} Just passing one child
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get parent index

   Warning: currently this implementation works,
            but if a tree requires more layers this implementation has to be adapted

   \param[in] orc_Index Index

   \return
   Parent index (may be invalid = root level)
*/
//----------------------------------------------------------------------------------------------------------------------
QModelIndex C_TblTreModel::parent(const QModelIndex & orc_Index) const
{
   QModelIndex c_Retval;

   if (orc_Index.isValid() == true)
   {
      //lint -e{925}  Result of Qt interface restrictions, set by index function
      const C_TblTreItem * const pc_TreeItem =
         static_cast<const C_TblTreItem * const>(orc_Index.internalPointer());
      if (pc_TreeItem != NULL)
      {
         if (pc_TreeItem->pc_Parent != NULL)
         {
            c_Retval = createIndex(pc_TreeItem->pc_Parent->GetIndexInParentNumber(), 0, pc_TreeItem->pc_Parent);
         }
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get tree row count

   \param[in] orc_Parent Parent

   \return
   Row count
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_TblTreModel::rowCount(const QModelIndex & orc_Parent) const
{
   sintn sn_Retval = 0;

   if (orc_Parent.isValid() == true)
   {
      //lint -e{925}  Result of Qt interface restrictions, set by index function
      const C_TblTreItem * const pc_TreeItem =
         static_cast<const C_TblTreItem * const>(orc_Parent.internalPointer());
      if (pc_TreeItem != NULL)
      {
         sn_Retval = pc_TreeItem->c_Children.size();
      }
   }
   else
   {
      if (this->mpc_InvisibleRootItem != NULL)
      {
         //Top level
         sn_Retval = this->mpc_InvisibleRootItem->c_Children.size();
      }
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
QVariant C_TblTreModel::data(const QModelIndex & orc_Index, const sintn osn_Role) const
{
   QVariant c_Retval;

   if ((orc_Index.isValid() == true) && (orc_Index.column() == 0))
   {
      if (osn_Role == static_cast<stw_types::sintn>(Qt::DisplayRole))
      {
         //lint -e{925}  Result of Qt interface restrictions, set by index function
         const C_TblTreItem * const pc_TreeItem =
            static_cast<const C_TblTreItem * const>(orc_Index.internalPointer());
         if (pc_TreeItem != NULL)
         {
            c_Retval = pc_TreeItem->c_Name;
         }
      }
      else if  (osn_Role == static_cast<stw_types::sintn>(Qt::DecorationRole))
      {
         //lint -e{925}  Result of Qt interface restrictions, set by index function
         const C_TblTreItem * const pc_TreeItem =
            static_cast<const C_TblTreItem * const>(orc_Index.internalPointer());
         if (pc_TreeItem != NULL)
         {
            c_Retval = pc_TreeItem->c_Icon;
         }
      }
      else if  (osn_Role == msn_USER_ROLE_TOOL_TIP_HEADING)
      {
         //lint -e{925}  Result of Qt interface restrictions, set by index function
         const C_TblTreItem * const pc_TreeItem =
            static_cast<const C_TblTreItem * const>(orc_Index.internalPointer());
         if (pc_TreeItem != NULL)
         {
            c_Retval = pc_TreeItem->c_ToolTipHeading;
         }
      }
      else if  (osn_Role == msn_USER_ROLE_TOOL_TIP_CONTENT)
      {
         //lint -e{925}  Result of Qt interface restrictions, set by index function
         const C_TblTreItem * const pc_TreeItem =
            static_cast<const C_TblTreItem * const>(orc_Index.internalPointer());
         if (pc_TreeItem != NULL)
         {
            c_Retval = pc_TreeItem->c_ToolTipContent;
         }
      }
      else
      {
         //Not necessary
      }
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get item flags

   \param[in] orc_Index Index

   \return
   Item flags
*/
//----------------------------------------------------------------------------------------------------------------------
Qt::ItemFlags C_TblTreModel::flags(const QModelIndex & orc_Index) const
{
   Qt::ItemFlags c_Retval = Qt::NoItemFlags;
   if (orc_Index.isValid() == true)
   {
      //lint -e{925}  Result of Qt interface restrictions, set by index function
      const C_TblTreItem * const pc_TreeItem =
         static_cast<const C_TblTreItem * const>(orc_Index.internalPointer());
      if (pc_TreeItem != NULL)
      {
         if (pc_TreeItem->q_Enabled == true)
         {
            c_Retval = c_Retval | Qt::ItemIsEnabled;
         }
         if (pc_TreeItem->q_Selectable == true)
         {
            c_Retval = c_Retval | Qt::ItemIsSelectable;
         }
      }
   }
   return c_Retval;
}
