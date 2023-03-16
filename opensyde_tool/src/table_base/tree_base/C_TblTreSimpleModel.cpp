//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Simple tree model base class

   Simple tree model base class

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_TblTreSimpleModel.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_gui_logic;

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

   \param[in]  os32_Row     Row
   \param[in]  os32_Column  Column
   \param[in]  orc_Parent   Parent

   \return
   Data index (may be invalid = invalid parameters)
*/
//----------------------------------------------------------------------------------------------------------------------
QModelIndex C_TblTreSimpleModel::index(const int32_t os32_Row, const int32_t os32_Column,
                                       const QModelIndex & orc_Parent) const
{
   QModelIndex c_Retval;

   if (hasIndex(os32_Row, os32_Column, orc_Parent) == true)
   {
      if (orc_Parent.isValid() == true)
      {
         //lint -e{9079}  Result of Qt interface restrictions, set by index function
         const C_TblTreSimpleItem * const pc_TreeItem =
            static_cast<const C_TblTreSimpleItem *>(orc_Parent.internalPointer());
         if (pc_TreeItem != NULL)
         {
            c_Retval =
               this->createIndex(os32_Row, os32_Column, pc_TreeItem->c_Children.at(static_cast<uint32_t>(os32_Row)));
         }
      }
      else
      {
         if (this->mpc_InvisibleRootItem != NULL)
         {
            //Top level
            c_Retval =
               this->createIndex(os32_Row, os32_Column,
                                 this->mpc_InvisibleRootItem->c_Children.at(static_cast<uint32_t>(os32_Row)));
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
            const int32_t s32_Index = pc_TreeItem->pc_Parent->GetIndexInParentNumber();
            if (s32_Index >= 0)
            {
               //Parent should always use column 0
               c_Retval = createIndex(s32_Index, 0, pc_TreeItem->pc_Parent);
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
int32_t C_TblTreSimpleModel::rowCount(const QModelIndex & orc_Parent) const
{
   int32_t s32_Retval = 0;

   if (orc_Parent.isValid() == true)
   {
      //lint -e{9079}  Result of Qt interface restrictions, set by index function
      const C_TblTreSimpleItem * const pc_TreeItem =
         static_cast<const C_TblTreSimpleItem *>(orc_Parent.internalPointer());
      if (pc_TreeItem != NULL)
      {
         s32_Retval = pc_TreeItem->c_Children.size();
      }
   }
   else
   {
      if (this->mpc_InvisibleRootItem != NULL)
      {
         //Top level
         s32_Retval = this->mpc_InvisibleRootItem->c_Children.size();
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Count layers

   \param[in]   orc_Index           Index
   \param[out]  oru32_ValidLayers   Number of valid layers, starts with 1 for the top layer

   \return
   STW error codes

   \retval   C_NO_ERR   Valid layers returned
   \retval   C_CONFIG   Could not determine number of valid layers
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_TblTreSimpleModel::m_CountLayers(const QModelIndex & orc_Index, uint32_t & oru32_ValidLayers) const
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_Index.isValid())
   {
      //lint -e{9079}  Result of Qt interface restrictions, set by index function
      const C_TblTreSimpleItem * const pc_TreeItem =
         static_cast<const C_TblTreSimpleItem *>(orc_Index.internalPointer());

      if (pc_TreeItem != NULL)
      {
         oru32_ValidLayers = 0UL;
         this->m_CountLayers(orc_Index.parent(), oru32_ValidLayers);
         ++oru32_ValidLayers;
      }
      else
      {
         s32_Retval = C_CONFIG;
      }
   }
   else
   {
      oru32_ValidLayers = 0UL;
   }
   return s32_Retval;
}
