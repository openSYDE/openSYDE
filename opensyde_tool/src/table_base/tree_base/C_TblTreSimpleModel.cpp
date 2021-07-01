//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Simple tree model base class

   Simple tree model base class

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_TblTreSimpleModel.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_TblTreSimpleModel::C_TblTreSimpleModel(QObject * const opc_Parent) :
   QAbstractItemModel(opc_Parent),
   mpc_InvisibleRootItem(NULL)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get data index

   \param[in]  osn_Row     Row
   \param[in]  osn_Column  Column
   \param[in]  orc_Parent  Parent

   \return
   Data index (may be invalid = invalid parameters)
*/
//----------------------------------------------------------------------------------------------------------------------
QModelIndex C_TblTreSimpleModel::index(const sintn osn_Row, const sintn osn_Column,
                                       const QModelIndex & orc_Parent) const
{
   QModelIndex c_Retval;

   if (hasIndex(osn_Row, osn_Column, orc_Parent) == true)
   {
      if (orc_Parent.isValid() == true)
      {
         //lint -e{9079}  Result of Qt interface restrictions, set by index function
         const C_TblTreSimpleItem * const pc_TreeItem =
            static_cast<const C_TblTreSimpleItem *>(orc_Parent.internalPointer());
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
/*! \brief  Get parent index

   Warning: currently this implementation works,
            but if a tree requires more layers this implementation has to be adapted

   \param[in]  orc_Index   Index

   \return
   Parent index (may be invalid = root level)
*/
//----------------------------------------------------------------------------------------------------------------------
QModelIndex C_TblTreSimpleModel::parent(const QModelIndex & orc_Index) const
{
   QModelIndex c_Retval;

   if (orc_Index.isValid() == true)
   {
      //lint -e{9079}  Result of Qt interface restrictions, set by index function
      const C_TblTreSimpleItem * const pc_TreeItem =
         static_cast<const C_TblTreSimpleItem *>(orc_Index.internalPointer());
      if (pc_TreeItem != NULL)
      {
         if (pc_TreeItem->pc_Parent != NULL)
         {
            const sint32 s32_Index = pc_TreeItem->pc_Parent->GetIndexInParentNumber();
            if (s32_Index >= 0)
            {
               c_Retval = createIndex(s32_Index, orc_Index.column(), pc_TreeItem->pc_Parent);
            }
         }
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get tree row count

   \param[in]  orc_Parent  Parent

   \return
   Row count
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_TblTreSimpleModel::rowCount(const QModelIndex & orc_Parent) const
{
   sintn sn_Retval = 0;

   if (orc_Parent.isValid() == true)
   {
      //lint -e{9079}  Result of Qt interface restrictions, set by index function
      const C_TblTreSimpleItem * const pc_TreeItem =
         static_cast<const C_TblTreSimpleItem *>(orc_Parent.internalPointer());
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
